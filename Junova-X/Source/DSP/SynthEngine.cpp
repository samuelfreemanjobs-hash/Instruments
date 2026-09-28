#include "SynthEngine.h"

namespace junovax::dsp
{
namespace
{
float midiNoteToHz (int note) noexcept
{
    return 440.0f * std::pow (2.0f, (static_cast<float> (note) - 69.0f) / 12.0f);
}

float normToCutoffHz (float norm, double sampleRate) noexcept
{
    const float minHz = 40.0f;
    const float maxHz = static_cast<float> (sampleRate * 0.45);
    return minHz * std::pow (maxHz / minHz, juce::jlimit (0.0f, 1.0f, norm));
}

float normToHpfHz (float norm, double sampleRate) noexcept
{
    const float minHz = 20.0f;
    const float maxHz = static_cast<float> (sampleRate * 0.2);
    return minHz * std::pow (maxHz / minHz, juce::jlimit (0.0f, 1.0f, norm));
}
} // namespace

void SynthEngine::prepare (double sampleRate, int maxBlockSize) noexcept
{
    juce::ignoreUnused (maxBlockSize);
    sampleRate_ = sampleRate;
    spec_.sampleRate = sampleRate;
    spec_.maximumBlockSize = static_cast<juce::uint32> (maxBlockSize);
    spec_.numChannels = 2;

    diagTone_.prepare (sampleRate);
    masterGain_.reset (sampleRate, 0.02);
    masterGain_.setCurrentAndTargetValue (1.0f);

    for (auto& v : voices_)
    {
        v.filter.prepare (spec_);
        v.filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        v.ampEnv.setSampleRate (sampleRate);
        v.filtEnv.setSampleRate (sampleRate);
    }

    hpfL_.prepare (spec_);
    hpfR_.prepare (spec_);
    chorus_.prepare (sampleRate);
    reset();
}

void SynthEngine::reset() noexcept
{
    diagTone_.reset();
    for (auto& v : voices_)
    {
        v.note = -1;
        v.keyDown = false;
        v.ampEnv.reset();
        v.filtEnv.reset();
        v.phase = 0.0f;
        v.subPhase = 0.0f;
    }
    lfoPhase_ = 0.0f;
    lfoDelaySamples_ = params_.lfoDelay * static_cast<float> (sampleRate_);
    chorus_.reset();
}

void SynthEngine::panic() noexcept
{
    reset();
    diagTone_.setEnabled (false);
}

void SynthEngine::setParams (const RuntimeParams& p) noexcept
{
    params_ = p;
    updateEnvelopes();
    const auto hpfHz = normToHpfHz (p.hpfCutoff, sampleRate_);
    auto hpfCoefs = juce::dsp::IIR::Coefficients<float>::makeHighPass (sampleRate_, hpfHz);
    *hpfL_.coefficients = *hpfCoefs;
    *hpfR_.coefficients = *hpfCoefs;
}

void SynthEngine::updateEnvelopes() noexcept
{
    juce::ADSR::Parameters amp;
    amp.attack = params_.ampAttack;
    amp.decay = params_.ampDecay;
    amp.sustain = params_.ampSustain;
    amp.release = params_.ampRelease;

    juce::ADSR::Parameters filt;
    filt.attack = params_.filtAttack;
    filt.decay = params_.filtDecay;
    filt.sustain = params_.filtSustain;
    filt.release = params_.filtRelease;

    for (auto& v : voices_)
    {
        v.ampEnv.setParameters (amp);
        v.filtEnv.setParameters (filt);
    }
}

int SynthEngine::findFreeVoice() noexcept
{
    for (int i = 0; i < kMaxVoices; ++i)
        if (voices_[static_cast<std::size_t> (i)].note < 0 && ! voices_[static_cast<std::size_t> (i)].ampEnv.isActive())
            return i;
    return 0;
}

int SynthEngine::findVoiceForNote (int note) noexcept
{
    for (int i = 0; i < kMaxVoices; ++i)
        if (voices_[static_cast<std::size_t> (i)].note == note)
            return i;
    return -1;
}

void SynthEngine::startVoice (Voice& v, int note, float velocity, bool retrigger) noexcept
{
    juce::ignoreUnused (velocity);
    const float target = midiNoteToHz (note);
    if (params_.voiceMode == 3 && ! retrigger)
    {
        monoGlideFreq_ = v.frequency;
        v.frequency = monoGlideFreq_;
    }
    else
    {
        v.frequency = target;
    }
    v.note = note;
    v.keyDown = true;
    if (retrigger || params_.voiceMode != 3)
    {
        v.ampEnv.noteOn();
        v.filtEnv.noteOn();
        v.phase = 0.0f;
        v.subPhase = 0.0f;
    }
    else
    {
        v.ampEnv.noteOn();
        v.filtEnv.noteOn();
    }
    monoActiveNote_ = note;
}

void SynthEngine::releaseVoice (Voice& v) noexcept
{
    v.keyDown = false;
    v.ampEnv.noteOff();
    v.filtEnv.noteOff();
}

void SynthEngine::handleMidi (const juce::MidiBuffer& midi) noexcept
{
    const bool unison = params_.voiceMode == 2;
    const bool mono = params_.voiceMode == 3;

    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            const int note = msg.getNoteNumber();
            const float vel = msg.getFloatVelocity();
            if (mono)
            {
                for (auto& v : voices_)
                    if (v.note >= 0)
                        releaseVoice (v);
                startVoice (voices_[0], note, vel, voices_[0].note < 0);
            }
            else if (unison)
            {
                for (int i = 0; i < kMaxVoices; ++i)
                {
                    auto& v = voices_[static_cast<std::size_t> (i)];
                    v.detuneCents = (static_cast<float> (i) - (kMaxVoices - 1) * 0.5f) * (params_.detune * 0.15f);
                    startVoice (v, note, vel, true);
                }
            }
            else
            {
                const int idx = findVoiceForNote (note);
                if (idx >= 0)
                    startVoice (voices_[static_cast<std::size_t> (idx)], note, vel, true);
                else
                    startVoice (voices_[static_cast<std::size_t> (findFreeVoice())], note, vel, true);
            }
        }
        else if (msg.isNoteOff())
        {
            const int note = msg.getNoteNumber();
            if (mono)
            {
                if (voices_[0].note == note)
                    releaseVoice (voices_[0]);
            }
            else if (unison)
            {
                for (auto& v : voices_)
                    if (v.note == note)
                        releaseVoice (v);
            }
            else if (const int idx = findVoiceForNote (note); idx >= 0)
            {
                releaseVoice (voices_[static_cast<std::size_t> (idx)]);
            }
        }
        else if (msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            panic();
        }
    }
}

float SynthEngine::cutoffHzForVoice (const Voice& v, float filtEnvLevel, float lfo) const noexcept
{
    const float base = normToCutoffHz (params_.filterCutoff, sampleRate_);
    const float envAmt = params_.vcfEnv * 0.01f;
    const float lfoAmt = params_.vcfLfo * 0.01f;
    const float keyTrack = params_.vcfKey * 0.01f;
    const float noteOffset = static_cast<float> (v.note - 60) / 12.0f;
    const float env = filtEnvLevel;
    float hz = base;
    hz *= std::pow (2.0f, envAmt * (env - 0.35f) * 4.0f);
    hz *= std::pow (2.0f, lfoAmt * lfo * 0.5f);
    hz *= std::pow (2.0f, keyTrack * noteOffset * 0.25f);
    return juce::jlimit (40.0f, static_cast<float> (sampleRate_ * 0.45), hz);
}

float SynthEngine::advanceLfo() noexcept
{
    if (lfoDelaySamples_ > 0.0f)
    {
        lfoDelaySamples_ -= 1.0f;
        return 0.0f;
    }
    const float inc = params_.lfoRate / static_cast<float> (sampleRate_);
    lfoPhase_ += inc;
    if (lfoPhase_ > 1.0f)
        lfoPhase_ -= 1.0f;
    return std::sin (lfoPhase_ * juce::MathConstants<float>::twoPi);
}

float SynthEngine::renderVoiceSample (Voice& v, float lfo) noexcept
{
    if (! v.ampEnv.isActive() && ! v.keyDown)
    {
        v.note = -1;
        return 0.0f;
    }

    if (params_.voiceMode == 3 && v.note >= 0)
    {
        const float target = midiNoteToHz (v.note);
        const float glide = juce::jmax (1.0f, params_.glideMs);
        const float coeff = 1.0f - std::exp (-1.0f / (static_cast<float> (sampleRate_) * glide * 0.001f));
        v.frequency += (target - v.frequency) * coeff;
    }

    const float pitchMod = 1.0f + (params_.dcoLfoMod * 0.01f) * lfo * 0.02f;
    const float detuneRatio = std::pow (2.0f, v.detuneCents / 1200.0f);
    const float drift = 1.0f + (params_.drift * 0.0001f) * (rng_.nextFloat() - 0.5f);
    const float freq = v.frequency * pitchMod * detuneRatio * drift;
    const float inc = freq / static_cast<float> (sampleRate_);

    v.phase += inc;
    if (v.phase >= 1.0f)
        v.phase -= 1.0f;
    v.subPhase += inc * 0.5f;
    if (v.subPhase >= 1.0f)
        v.subPhase -= 1.0f;

    const float pwm = juce::jlimit (0.05f, 0.95f, params_.dcoPwm * 0.01f);
    const float saw = 2.0f * v.phase - 1.0f;
    const float pulse = v.phase < pwm ? 1.0f : -1.0f;
    const float osc = 0.65f * saw + 0.35f * pulse;

    const float sub = std::sin (v.subPhase * juce::MathConstants<float>::twoPi);
    const float noise = (rng_.nextFloat() * 2.0f - 1.0f) * (params_.dcoNoise * 0.01f);
    const float subMix = params_.dcoSubLvl * 0.01f;

    float raw = osc + sub * subMix + noise;

    const float filtLevel = v.filtEnv.getNextSample();
    const float cutoff = cutoffHzForVoice (v, filtLevel, lfo);
    v.filter.setCutoffFrequency (cutoff);
    v.filter.setResonance (0.707f + params_.filterRes * 3.5f);
    raw = v.filter.processSample (0, raw);

    return raw * v.ampEnv.getNextSample() * 0.12f;
}

void SynthEngine::render (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) noexcept
{
    handleMidi (midi);

    if (params_.diagTestTone)
    {
        diagTone_.setEnabled (true);
        diagTone_.setFrequencyHz (params_.diagToneFreqHz);
        const float gain = juce::Decibels::decibelsToGain (params_.masterGainDb);
        masterGain_.setTargetValue (gain);
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* data = buffer.getWritePointer (ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                data[i] = diagTone_.processSample() * masterGain_.getNextValue();
        }
        return;
    }

    diagTone_.setEnabled (false);
    const float gain = juce::Decibels::decibelsToGain (params_.masterGainDb);
    masterGain_.setTargetValue (gain);

    const int numSamples = buffer.getNumSamples();
    const int numCh = buffer.getNumChannels();
    auto* left = buffer.getWritePointer (0);
    float* right = numCh > 1 ? buffer.getWritePointer (1) : left;

    for (int i = 0; i < numSamples; ++i)
    {
        const float lfo = advanceLfo();
        float mix = 0.0f;
        for (auto& v : voices_)
            mix += renderVoiceSample (v, lfo);

        const float width = juce::jlimit (0.0f, 2.0f, params_.width * 0.01f);
        float l = mix * (1.0f - 0.15f * width);
        float r = mix * (1.0f + 0.15f * width);
        chorus_.process (l, r, params_.chorusMode);

        if (params_.hpfEnabled)
        {
            l = hpfL_.processSample (l);
            r = hpfR_.processSample (r);
        }

        const float g = masterGain_.getNextValue();
        left[i] = l * g;
        if (numCh > 1)
            right[i] = r * g;
    }
}
} // namespace junovax::dsp
