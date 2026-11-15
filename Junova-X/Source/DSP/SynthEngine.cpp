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

float hpfHzForMode (int mode, double sampleRate) noexcept
{
    juce::ignoreUnused (sampleRate);
    switch (juce::jlimit (0, 3, mode))
    {
        case 1:
            return 80.0f;
        case 2:
            return 120.0f;
        case 3:
            return 220.0f;
        case 0:
        default:
            return 20.0f;
    }
}
} // namespace

void SynthEngine::prepare (double sampleRate, int maxBlockSize) noexcept
{
    juce::ignoreUnused (maxBlockSize);
    sampleRate_ = sampleRate;
    spec_.sampleRate = sampleRate;
    spec_.maximumBlockSize = static_cast<juce::uint32> (maxBlockSize);
    spec_.numChannels = 1;

    diagTone_.prepare (sampleRate);
    masterGain_.reset (sampleRate, 0.02);
    masterGain_.setCurrentAndTargetValue (1.0f);
    initParamSmoothers();

    for (auto& v : voices_)
    {
        v.filter.prepare (sampleRate);
        v.ampEnv.setSampleRate (sampleRate);
        v.filtEnv.setSampleRate (sampleRate);
        v.analogDrift.seed (rng_);
    }

    hpfL_.prepare (spec_);
    hpfR_.prepare (spec_);
    chorus_.prepare (sampleRate);
    rng_.setSeed (0x4a434u);
    reset();
}

void SynthEngine::initParamSmoothers() noexcept
{
    const double smoothSec = 0.01;
    smFilterCutoff_.reset (sampleRate_, smoothSec);
    smFilterRes_.reset (sampleRate_, smoothSec);
    smPwm_.reset (sampleRate_, smoothSec);
    smSubLvl_.reset (sampleRate_, smoothSec);
    smNoise_.reset (sampleRate_, smoothSec);
    smVcfDrive_.reset (sampleRate_, smoothSec);
    smFilterCutoff_.setCurrentAndTargetValue (params_.filterCutoff);
    smFilterRes_.setCurrentAndTargetValue (params_.filterRes);
    smPwm_.setCurrentAndTargetValue (params_.dcoPwm);
    smSubLvl_.setCurrentAndTargetValue (params_.dcoSubLvl);
    smNoise_.setCurrentAndTargetValue (params_.dcoNoise);
    smVcfDrive_.setCurrentAndTargetValue (params_.vcfDrive);
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
        v.lastOscSample = 0.0f;
        v.filter.reset();
        v.analogDrift.seed (rng_);
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
    smFilterCutoff_.setTargetValue (p.filterCutoff);
    smFilterRes_.setTargetValue (p.filterRes);
    smPwm_.setTargetValue (p.dcoPwm);
    smSubLvl_.setTargetValue (p.dcoSubLvl);
    smNoise_.setTargetValue (p.dcoNoise);
    smVcfDrive_.setTargetValue (p.vcfDrive);
    if (p.hpfMode > 0)
    {
        const auto hpfHz = hpfHzForMode (p.hpfMode, sampleRate_);
        auto hpfCoefs = juce::dsp::IIR::Coefficients<float>::makeHighPass (sampleRate_, hpfHz);
        *hpfL_.coefficients = *hpfCoefs;
        *hpfR_.coefficients = *hpfCoefs;
    }
}

void SynthEngine::updateEnvelopes() noexcept
{
    for (auto& v : voices_)
    {
        v.ampEnv.setParameters (params_.ampAttack, params_.ampDecay, params_.ampSustain, params_.ampRelease);
        v.filtEnv.setParameters (params_.filtAttack, params_.filtDecay, params_.filtSustain, params_.filtRelease);
    }
}

int SynthEngine::maxPolyVoices() const noexcept
{
    if (params_.voiceMode == 4)
        return 6;
    return kMaxVoices;
}

int SynthEngine::filterOversampleFactor() const noexcept
{
    if (params_.oversampleMode >= 2)
        return 4;
    if (params_.oversampleMode >= 1)
        return 2;
    return 1;
}

float SynthEngine::readAndResetPeak() noexcept
{
    const float p = peakHold_;
    peakHold_ = 0.0f;
    return p;
}

std::uint8_t SynthEngine::getActiveVoiceMask() const noexcept
{
    std::uint8_t mask = 0;
    const int limit = maxPolyVoices();
    for (int i = 0; i < limit; ++i)
    {
        const auto& v = voices_[static_cast<std::size_t> (i)];
        if (v.ampEnv.isActive() || v.keyDown)
            mask = static_cast<std::uint8_t> (mask | (1u << i));
    }
    return mask;
}

float SynthEngine::polyBlep (float phase, float inc) const noexcept
{
    if (inc <= 0.0f)
        return 0.0f;
    if (phase < inc)
    {
        const float t = phase / inc;
        return t + t - t * t - 1.0f;
    }
    if (phase + inc > 1.0f)
    {
        const float t = (phase - 1.0f) / inc;
        return t * t + t + t + 1.0f;
    }
    return 0.0f;
}

float SynthEngine::blepPulse (float phase, float inc, float pw) const noexcept
{
    pw = juce::jlimit (0.05f, 0.95f, pw);
    float out = phase < pw ? 1.0f : -1.0f;
    out -= polyBlep (phase, inc);
    const float phase2 = std::fmod (phase - pw + 1.0f, 1.0f);
    out += polyBlep (phase2, inc);
    return out;
}

float SynthEngine::lfoValueAtPhase (float phase) const noexcept
{
    switch (params_.lfoWaveform)
    {
        case 1:
            return phase < 0.5f ? 1.0f : -1.0f;
        case 2:
            return lfoRandHold_;
        case 0:
        default:
            return std::sin (phase * juce::MathConstants<float>::twoPi);
    }
}

int SynthEngine::findFreeVoice() noexcept
{
    const int limit = maxPolyVoices();
    for (int i = 0; i < limit; ++i)
    {
        const auto& v = voices_[static_cast<std::size_t> (i)];
        if (v.note < 0 && ! v.ampEnv.isActive())
            return i;
    }
    for (int i = 0; i < limit; ++i)
    {
        const auto& v = voices_[static_cast<std::size_t> (i)];
        if (! v.keyDown && ! v.ampEnv.isActive())
            return i;
    }
    return 0;
}

int SynthEngine::findVoiceForNote (int note) noexcept
{
    const int limit = maxPolyVoices();
    for (int i = 0; i < limit; ++i)
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
    const int prevNote = v.note;
    v.note = note;
    v.keyDown = true;
    v.analogDrift.seed (rng_);
    const bool sameNoteRetrig = retrigger && note == prevNote;
    if (! sameNoteRetrig)
    {
        v.phase = 0.0f;
        v.subPhase = 0.0f;
        v.lastOscSample = 0.0f;
    }
    v.ampEnv.noteOn();
    v.filtEnv.noteOn();
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

float SynthEngine::cutoffHzForVoice (const Voice& v, float filtEnvLevel, float lfo, float smCutoff) const noexcept
{
    const float base = normToCutoffHz (smCutoff, sampleRate_) * v.analogDrift.cutoffMul (params_.drift);
    const float envAmt = params_.vcfEnv * 0.01f;
    const float lfoAmt = params_.vcfLfo * 0.01f;
    const float keyTrack = params_.vcfKey * 0.01f;
    const float noteOffset = static_cast<float> (v.note - 60) / 12.0f;
  #if ! JUCE_MINGW
    const float env = params_.vcfEnvInv ? (1.0f - filtEnvLevel) : filtEnvLevel;
  #else
    const float env = filtEnvLevel;
  #endif
    float hz = base;
    const float envScale = params_.envVintageCurve ? 3.2f : 4.0f;
    hz *= std::pow (2.0f, envAmt * (env - 0.35f) * envScale);
    hz *= std::pow (2.0f, lfoAmt * lfo * 0.5f);
    hz *= std::pow (2.0f, keyTrack * noteOffset * 0.25f);
    return juce::jlimit (40.0f, static_cast<float> (sampleRate_ * 0.45), hz);
}

float SynthEngine::filterVoiceSample (Voice& v, float raw, float cutoffHz, float smRes) noexcept
{
    const float res = juce::jlimit (0.0f, 1.0f, smRes);
    const float drive = 1.0f + smVcfDrive_.getCurrentValue() * 0.04f + res * 1.6f;
    v.filter.setTargets (cutoffHz, res, drive);

    const int os = filterOversampleFactor();
    float out = raw;
    if (os <= 1)
        out = v.filter.processSample (raw);
    else
    {
        const float step = 1.0f / static_cast<float> (os);
        float acc = v.lastOscSample;
        for (int i = 0; i < os; ++i)
        {
            const float t = static_cast<float> (i + 1) * step;
            const float upsampled = v.lastOscSample + (raw - v.lastOscSample) * t;
            acc = v.filter.processSample (upsampled);
        }
        out = acc;
    }
    v.lastOscSample = raw;
    return out;
}

float SynthEngine::advanceLfo() noexcept
{
    if (lfoDelaySamples_ > 0.0f)
    {
        lfoDelaySamples_ -= 1.0f;
        return 0.0f;
    }
    float hz = params_.lfoRate;
    if (params_.lfoBpmSync)
        hz = params_.hostBpm / 60.0f * (params_.lfoRate / 8.0f);
    const float inc = hz / static_cast<float> (sampleRate_);
    lfoPhase_ += inc;
    if (lfoPhase_ > 1.0f)
    {
        lfoPhase_ -= 1.0f;
        if (params_.lfoWaveform == 2)
            lfoRandHold_ = rng_.nextFloat() * 2.0f - 1.0f;
    }
    return lfoValueAtPhase (lfoPhase_);
}

float SynthEngine::renderVoiceSample (Voice& v, float lfo, float smCutoff, float smRes, float smPwm) noexcept
{
    if (! v.ampEnv.isActive() && ! v.keyDown)
    {
        v.note = -1;
        return 0.0f;
    }

    v.analogDrift.advance (sampleRate_, params_.drift, rng_);

    if (params_.voiceMode == 3 && v.note >= 0)
    {
        const float target = midiNoteToHz (v.note);
        const float glide = juce::jmax (1.0f, params_.glideMs);
        const float coeff = 1.0f - std::exp (-1.0f / (static_cast<float> (sampleRate_) * glide * 0.001f));
        v.frequency += (target - v.frequency) * coeff;
    }

    static constexpr float kOctaveMul[] = { 0.25f, 0.5f, 1.0f, 2.0f };
    const int octIdx = juce::jlimit (0, 3, params_.dcoOctave);
    const float octaveMul = kOctaveMul[static_cast<std::size_t> (octIdx)];
    const float pitchMod = 1.0f + (params_.dcoLfoMod * 0.01f) * lfo * 0.02f;
    const float detuneRatio = std::pow (2.0f, v.detuneCents / 1200.0f);
    const float freq = v.frequency * pitchMod * detuneRatio * v.analogDrift.pitchRatio (params_.drift) * octaveMul;
    const float inc = freq / static_cast<float> (sampleRate_);

    v.phase += inc;
    if (v.phase >= 1.0f)
        v.phase -= 1.0f;
    const float subDiv = params_.dcoSubOct == 1 ? 0.25f : 0.5f;
    v.subPhase += inc * subDiv;
    if (v.subPhase >= 1.0f)
        v.subPhase -= 1.0f;

    const float pwmManual = juce::jlimit (0.05f, 0.95f, smPwm * 0.01f);
    const float pwm = params_.pwmSourceLfo
                          ? juce::jlimit (0.05f, 0.95f, 0.5f + lfo * smPwm * 0.004f)
                          : pwmManual;

    float saw = 0.0f;
    if (params_.dcoSawOn)
    {
        saw = 2.0f * v.phase - 1.0f;
        if (! params_.measurementLegacyOsc)
            saw -= polyBlep (v.phase, inc);
    }

    float pulse = 0.0f;
    if (params_.dcoPulseOn)
    {
        pulse = params_.measurementLegacyOsc ? (v.phase < pwm ? 1.0f : -1.0f) : blepPulse (v.phase, inc, pwm);
    }

    float osc = 0.0f;
    if (params_.dcoSawOn && params_.dcoPulseOn)
        osc = 0.52f * saw + 0.48f * pulse;
    else if (params_.dcoSawOn)
        osc = saw;
    else if (params_.dcoPulseOn)
        osc = pulse;

    float sub = 0.0f;
    if (params_.dcoSubOn)
    {
        sub = v.subPhase < 0.5f ? 1.0f : -1.0f;
        sub -= polyBlep (v.subPhase, inc * subDiv);
    }

    const float rawNoise = rng_.nextFloat() * 2.0f - 1.0f;
    v.noiseLp += 0.08f * (rawNoise - v.noiseLp);
    const float noiseAmt = smNoise_.getCurrentValue() * 0.0075f;
    const float subMix = smSubLvl_.getCurrentValue() * 0.0038f;

    float raw = osc + sub * subMix + v.noiseLp * noiseAmt;

    const float filtLevel = v.filtEnv.getNextSample();
    const float cutoff = cutoffHzForVoice (v, filtLevel, lfo, smCutoff);
    raw = filterVoiceSample (v, raw, cutoff, smRes);

    const float gateOrEnv = params_.vcaGateMode
                                ? (v.keyDown ? 1.0f : 0.0f)
                                : v.ampEnv.getNextSample();
    return raw * gateOrEnv * 0.105f;
}

void SynthEngine::render (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) noexcept
{
    juce::ScopedNoDenormals noDenormals;
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

    float blockPeak = 0.0f;
  #if ! JUCE_MINGW
    const float ceiling = juce::Decibels::decibelsToGain (-0.5f);
  #else
    const float ceiling = 0.9441f;
  #endif

    float dcAccumulator = 0.0f;
    const float dcCoeff = 1.0f - std::exp (-2.0f * juce::MathConstants<float>::pi * 18.0f / static_cast<float> (sampleRate_));

    for (int i = 0; i < numSamples; ++i)
    {
        const float smCutoff = smFilterCutoff_.getNextValue();
        const float smRes = smFilterRes_.getNextValue();
        const float smPwm = smPwm_.getNextValue();
        smSubLvl_.getNextValue();
        smNoise_.getNextValue();
        smVcfDrive_.getNextValue();

        const float lfo = advanceLfo();
        float mix = 0.0f;
        for (auto& v : voices_)
            mix += renderVoiceSample (v, lfo, smCutoff, smRes, smPwm);

        const float width = juce::jlimit (0.0f, 2.0f, params_.width * 0.01f);
        float l = mix * (1.0f - 0.15f * width);
        float r = mix * (1.0f + 0.15f * width);
        chorus_.process (l, r, params_.chorusMode);
        if (params_.chorusHiss)
        {
            const float hiss = (rng_.nextFloat() * 2.0f - 1.0f) * 0.00015f;
            l += hiss;
            r += hiss;
        }

        if (params_.hpfMode > 0)
        {
            l = hpfL_.processSample (l);
            r = hpfR_.processSample (r);
        }

        dcAccumulator += dcCoeff * (l - dcAccumulator);
        l -= dcAccumulator * 0.35f;
        r -= dcAccumulator * 0.35f;

        const float g = masterGain_.getNextValue();
        left[i] = l * g;
        if (numCh > 1)
            right[i] = r * g;

        blockPeak = juce::jmax (blockPeak, std::abs (left[i]));
        if (numCh > 1)
            blockPeak = juce::jmax (blockPeak, std::abs (right[i]));
    }

    peakHold_ = blockPeak;
    if (blockPeak > ceiling)
    {
        const float trim = ceiling / (blockPeak + 1.0e-12f);
        for (int i = 0; i < numSamples; ++i)
        {
            left[i] *= trim;
            if (numCh > 1)
                right[i] *= trim;
        }
        peakHold_ = blockPeak * trim;
    }
}
} // namespace junovax::dsp