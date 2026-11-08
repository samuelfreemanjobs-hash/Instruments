#include "SynthProcessor.h"
#include "SynthEditor.h"
#include "MoogStackLineage.h"
#include "VoyagerPresetBank.h"

namespace
{
constexpr int kNumVoices = 1;
constexpr float kTwoPi = juce::MathConstants<float>::twoPi;

float polyBlep(float phase, float dt) noexcept
{
    if (dt <= 0.0f)
        return 0.0f;
    if (phase < dt)
    {
        const float t = phase / dt;
        return t + t - t * t - 1.0f;
    }
    if (phase > 1.0f - dt)
    {
        const float t = (phase - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }
    return 0.0f;
}

float dbToLinear(float db) noexcept
{
    return juce::Decibels::decibelsToGain(db, -100.0f);
}

juce::StringArray waveChoiceLabels()
{
    return { "Saw", "Square", "Triangle", "Pulse" };
}

juce::StringArray octaveChoiceLabels()
{
    return { "-2", "-1", "0", "+1", "+2" };
}
} // namespace

MoogVoyagerAudioProcessor::SynthVoice::SynthVoice(MoogVoyagerAudioProcessor& ownerProcessor)
    : owner(ownerProcessor)
{
}

void MoogVoyagerAudioProcessor::SynthVoice::prepare(double sampleRate, int maxBlockSize)
{
    juce::ignoreUnused(maxBlockSize);
    ladderSpec.sampleRate = sampleRate;
    ladderSpec.maximumBlockSize = static_cast<juce::uint32>(juce::jmax(512, maxBlockSize));
    ladderSpec.numChannels = 1;
    ladder.prepare(ladderSpec);
    ladder.setMode(juce::dsp::LadderFilterMode::LPF24);
    ladder.setEnabled(true);
    ladder.reset();
    filterAdsr.setSampleRate(sampleRate);
    ampAdsr.setSampleRate(sampleRate);
    prepared = true;
}

bool MoogVoyagerAudioProcessor::SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

float MoogVoyagerAudioProcessor::SynthVoice::noteToHz(int midiNote, float octaveOffset, float fineCents) noexcept
{
    const float semitones = static_cast<float>(midiNote - 69) + octaveOffset * 12.0f + fineCents / 100.0f;
    return 440.0f * std::pow(2.0f, semitones / 12.0f);
}

float MoogVoyagerAudioProcessor::SynthVoice::mixerSaturate(float sample, float drive) noexcept
{
    const float d = juce::jmax(1.0f, drive);
    return std::tanh(sample * d) / std::tanh(d);
}

float MoogVoyagerAudioProcessor::SynthVoice::computeCutoffHz(const VoiceParams& params, float filterEnv,
                                                             float lfo, int midiNote) noexcept
{
    const float noteSemis = static_cast<float>(midiNote - 69) * params.filterKeyTrack;
    const float envSemis = filterEnv * params.filterEnvAmount * 5.5f;
    const float lfoSemis = lfo * params.lfoToFilter * 2.0f;
    const float semis = noteSemis + envSemis + lfoSemis;
    return juce::jlimit(20.0f, 20000.0f, params.filterCutoffHz * std::pow(2.0f, semis / 12.0f));
}

float MoogVoyagerAudioProcessor::SynthVoice::waveFromPhase(float phase01, float phaseIncrement,
                                                           int waveIndex) noexcept
{
    const float dt = juce::jlimit(0.0f, 0.5f, phaseIncrement);
    switch (waveIndex)
    {
        case 1:
        {
            const float sq = phase01 < 0.5f ? 1.0f : -1.0f;
            return sq + polyBlep(phase01, dt) - polyBlep(std::fmod(phase01 + 0.5f, 1.0f), dt);
        }
        case 2:
            return 1.0f - 4.0f * std::abs(phase01 - 0.5f);
        case 3:
            return phase01 < 0.25f ? 1.0f : -1.0f;
        case 0:
        default:
            return (2.0f * phase01 - 1.0f) - polyBlep(phase01, dt);
    }
}

float MoogVoyagerAudioProcessor::SynthVoice::nextNoiseSample(bool pink) noexcept
{
    rngState = rngState * 1664525u + 1013904223u;
    const float white = (static_cast<int32_t>(rngState) / static_cast<float>(INT32_MAX)) * 2.0f - 1.0f;
    if (!pink)
        return white;
    pinkNoiseState = pinkNoiseState * 0.992f + white * 0.055f;
    return juce::jlimit(-1.0f, 1.0f, pinkNoiseState * 2.2f);
}

void MoogVoyagerAudioProcessor::SynthVoice::startNote(int midiNoteNumber, float velocity,
                                                      juce::SynthesiserSound*, int)
{
    if (!prepared)
        prepare(getSampleRate(), 512);

    const auto params = owner.readVoiceParams();
    currentMidiNote = midiNoteNumber;
    velocityGain = juce::jlimit(0.0f, 1.0f, velocity);

    const float newTarget = noteToHz(midiNoteNumber, 0.0f, 0.0f);
    targetHz = newTarget;
    if (params.glideSeconds <= 0.001f || !isVoiceActive())
        currentHz = newTarget;
    else
    {
        const float sr = static_cast<float>(getSampleRate());
        glideCoeff = 1.0f - std::exp(-1.0f / (params.glideSeconds * sr));
    }

    filterAdsr.setParameters(params.filterEnv);
    ampAdsr.setParameters(params.ampEnv);
    filterAdsr.noteOn();
    ampAdsr.noteOn();
}

void MoogVoyagerAudioProcessor::SynthVoice::stopNote(float, bool allowTailOff)
{
    if (allowTailOff)
    {
        filterAdsr.noteOff();
        ampAdsr.noteOff();
    }
    else
    {
        filterAdsr.reset();
        ampAdsr.reset();
        clearCurrentNote();
        currentHz = 0.0f;
        targetHz = 0.0f;
        ladder.reset();
    }
}

void MoogVoyagerAudioProcessor::SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                                            int startSample,
                                                            int numSamples)
{
    if (!isVoiceActive())
        return;

    const auto params = owner.getCachedVoiceParams();
    const float sr = static_cast<float>(getSampleRate());
    const float lfoInc = params.lfoRateHz / sr;
    constexpr float driftRates[3] = { 0.07f, 0.11f, 0.05f };

    while (--numSamples >= 0)
    {
        if (std::abs(currentHz - targetHz) > 0.01f)
            currentHz += (targetHz - currentHz) * glideCoeff;
        else
            currentHz = targetHz;

        const float lfo = std::sin(lfoPhase * kTwoPi);
        lfoPhase += lfoInc;
        if (lfoPhase >= 1.0f)
            lfoPhase -= 1.0f;

        const float lfoPitchMul = std::pow(2.0f, lfo * params.lfoToPitch * 2.0f / 12.0f);
        const float rootHz = juce::jmax(20.0f, currentHz * lfoPitchMul);

        float mixed = 0.0f;
        for (int o = 0; o < 3; ++o)
        {
            driftPhase[o] += driftRates[o] / sr;
            const float drift =
                1.0f + 0.0012f * params.stackDrift * std::sin(driftPhase[o] * kTwoPi);
            const float hz = rootHz * std::pow(2.0f, params.oscOctave[o])
                             * std::pow(2.0f, params.oscFine[o] / 1200.0f) * drift;
            const float inc = hz / sr;
            phase[o] += inc;
            if (phase[o] >= 1.0f)
                phase[o] -= 1.0f;
            mixed += waveFromPhase(phase[o], inc, params.oscWave[o]) * params.oscLevel[o];
        }
        mixed += nextNoiseSample(params.noisePink) * params.noiseLevel;

        if (params.subOscLevel > 0.001f)
        {
            const float subHz = rootHz * 0.5f;
            const float subInc = subHz / sr;
            subPhase += subInc;
            if (subPhase >= 1.0f)
                subPhase -= 1.0f;
            const float subSq = subPhase < 0.5f ? 1.0f : -1.0f;
            mixed += subSq * params.subOscLevel;
        }

        const float mixerDrive = params.mixerDrive * params.stackMixerScale * (params.mixerOd ? 1.55f : 1.0f);
        mixed = mixerSaturate(mixed, mixerDrive);

        const float filterEnv = filterAdsr.getNextSample();
        const float ampEnv = ampAdsr.getNextSample();

        const float cutoff = computeCutoffHz(params, filterEnv, lfo, currentMidiNote) * params.stackCutoffScale;

        ladder.setCutoffFrequencyHz(cutoff);
        ladder.setResonance(juce::jlimit(0.0f, 1.0f, params.filterResonance + params.stackResBias));
        const float ladderDrive = params.filterDrive * params.stackFilterDriveScale * (params.filterOd ? 1.6f : 1.0f);
        ladder.setDrive(ladderDrive);

        float filterIn = mixed;
        float* channelPtr = &filterIn;
        juce::dsp::AudioBlock<float> sampleBlock(&channelPtr, 1, 1);
        juce::dsp::ProcessContextReplacing<float> filterContext(sampleBlock);
        ladder.process(filterContext);
        const float filtered = filterIn;
        const float out = filtered * ampEnv * velocityGain * 0.35f;

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample(ch, startSample, out);

        ++startSample;

        if (!filterAdsr.isActive() && !ampAdsr.isActive())
        {
            clearCurrentNote();
            ladder.reset();
            break;
        }
    }
}

MoogVoyagerAudioProcessor::VoiceParams MoogVoyagerAudioProcessor::readVoiceParams() const noexcept
{
    VoiceParams p;
    auto* osc1Oct = apvts.getRawParameterValue(std::string(SynthParamIDs::osc1Octave));
    auto* osc2Oct = apvts.getRawParameterValue(std::string(SynthParamIDs::osc2Octave));
    auto* osc3Oct = apvts.getRawParameterValue(std::string(SynthParamIDs::osc3Octave));
    auto* osc1W = apvts.getRawParameterValue(std::string(SynthParamIDs::osc1Wave));
    auto* osc2W = apvts.getRawParameterValue(std::string(SynthParamIDs::osc2Wave));
    auto* osc3W = apvts.getRawParameterValue(std::string(SynthParamIDs::osc3Wave));

    p.oscLevel[0] = apvts.getRawParameterValue(std::string(SynthParamIDs::osc1Level))->load();
    p.oscLevel[1] = apvts.getRawParameterValue(std::string(SynthParamIDs::osc2Level))->load();
    p.oscLevel[2] = apvts.getRawParameterValue(std::string(SynthParamIDs::osc3Level))->load();
    p.oscFine[0] = apvts.getRawParameterValue(std::string(SynthParamIDs::osc1Fine))->load();
    p.oscFine[1] = apvts.getRawParameterValue(std::string(SynthParamIDs::osc2Fine))->load();
    p.oscFine[2] = apvts.getRawParameterValue(std::string(SynthParamIDs::osc3Fine))->load();
    p.noiseLevel = apvts.getRawParameterValue(std::string(SynthParamIDs::noiseLevel))->load();

    const auto octaveFromChoice = [](float choiceIndex) { return choiceIndex - 2.0f; };
    p.oscOctave[0] = octaveFromChoice(osc1Oct->load());
    p.oscOctave[1] = octaveFromChoice(osc2Oct->load());
    p.oscOctave[2] = octaveFromChoice(osc3Oct->load());
    p.oscWave[0] = static_cast<int>(osc1W->load());
    p.oscWave[1] = static_cast<int>(osc2W->load());
    p.oscWave[2] = static_cast<int>(osc3W->load());

    p.filterCutoffHz = apvts.getRawParameterValue(std::string(SynthParamIDs::filterCutoff))->load();
    p.filterResonance = apvts.getRawParameterValue(std::string(SynthParamIDs::filterResonance))->load();
    p.filterDrive = apvts.getRawParameterValue(std::string(SynthParamIDs::filterDrive))->load();
    p.filterEnvAmount = apvts.getRawParameterValue(std::string(SynthParamIDs::filterEnvAmount))->load();
    p.filterKeyTrack = apvts.getRawParameterValue(std::string(SynthParamIDs::filterKeyTrack))->load();

    p.filterEnv.attack = apvts.getRawParameterValue(std::string(SynthParamIDs::filterAttack))->load();
    p.filterEnv.decay = apvts.getRawParameterValue(std::string(SynthParamIDs::filterDecay))->load();
    p.filterEnv.sustain = apvts.getRawParameterValue(std::string(SynthParamIDs::filterSustain))->load();
    p.filterEnv.release = apvts.getRawParameterValue(std::string(SynthParamIDs::filterRelease))->load();

    p.ampEnv.attack = apvts.getRawParameterValue(std::string(SynthParamIDs::ampAttack))->load();
    p.ampEnv.decay = apvts.getRawParameterValue(std::string(SynthParamIDs::ampDecay))->load();
    p.ampEnv.sustain = apvts.getRawParameterValue(std::string(SynthParamIDs::ampSustain))->load();
    p.ampEnv.release = apvts.getRawParameterValue(std::string(SynthParamIDs::ampRelease))->load();

    p.lfoRateHz = apvts.getRawParameterValue(std::string(SynthParamIDs::lfoRate))->load();
    p.lfoToPitch = apvts.getRawParameterValue(std::string(SynthParamIDs::lfoToPitch))->load();
    p.lfoToFilter = apvts.getRawParameterValue(std::string(SynthParamIDs::lfoToFilter))->load();
    p.glideSeconds = apvts.getRawParameterValue(std::string(SynthParamIDs::glideTime))->load();
    p.mixerDrive = apvts.getRawParameterValue(std::string(SynthParamIDs::mixerDrive))->load();
    p.subOscLevel = apvts.getRawParameterValue(std::string(SynthParamIDs::subOscLevel))->load();

    const int stackIdx = static_cast<int>(apvts.getRawParameterValue(std::string(SynthParamIDs::moogStack))->load());
    p.moogStack = stackIdx;
    const auto lineage = moogstack::lineageFromChoiceIndex(stackIdx);
    const auto c = moogstack::coeffsFor(lineage);
    p.stackDrift = c.driftAmount;
    p.stackCutoffScale = c.cutoffScale;
    p.stackResBias = c.resonanceBias;
    p.stackMixerScale = c.mixerDriveScale;
    p.stackFilterDriveScale = c.filterDriveScale;

    p.noisePink = apvts.getRawParameterValue(std::string(SynthParamIDs::noisePink))->load() >= 0.5f;
    p.mixerOd = apvts.getRawParameterValue(std::string(SynthParamIDs::mixerOd))->load() >= 0.5f;
    p.filterOd = apvts.getRawParameterValue(std::string(SynthParamIDs::filterOd))->load() >= 0.5f;

    return p;
}

juce::AudioProcessorValueTreeState::ParameterLayout MoogVoyagerAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    const auto waves = waveChoiceLabels();
    const auto octaves = octaveChoiceLabels();

    auto addFloat = [&](std::string_view id, const juce::String& name, float minV, float maxV, float def,
                        float skew = 1.0f) {
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { std::string(id), 1 },
            name,
            juce::NormalisableRange<float> { minV, maxV, 0.001f, skew },
            def));
    };

    addFloat(SynthParamIDs::outputGainDb, "Master", -24.0f, 12.0f, -3.0f);

    for (int i = 1; i <= 3; ++i)
    {
        const juce::String prefix = "OSC " + juce::String(i) + " ";
        const std::string_view level = (i == 1)   ? SynthParamIDs::osc1Level
                                       : (i == 2) ? SynthParamIDs::osc2Level
                                                  : SynthParamIDs::osc3Level;
        const std::string_view oct = (i == 1)   ? SynthParamIDs::osc1Octave
                                   : (i == 2) ? SynthParamIDs::osc2Octave
                                              : SynthParamIDs::osc3Octave;
        const std::string_view fine = (i == 1)   ? SynthParamIDs::osc1Fine
                                    : (i == 2) ? SynthParamIDs::osc2Fine
                                               : SynthParamIDs::osc3Fine;
        const std::string_view wave = (i == 1)   ? SynthParamIDs::osc1Wave
                                    : (i == 2) ? SynthParamIDs::osc2Wave
                                               : SynthParamIDs::osc3Wave;

        addFloat(level, prefix + "Level", 0.0f, 1.0f, i == 1 ? 0.85f : 0.55f);
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID { std::string(oct), 1 },
            prefix + "Octave",
            octaves,
            2));
        addFloat(fine, prefix + "Fine", -100.0f, 100.0f, 0.0f);
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID { std::string(wave), 1 },
            prefix + "Wave",
            waves,
            i == 3 ? 2 : 0));
    }

    addFloat(SynthParamIDs::noiseLevel, "Noise", 0.0f, 1.0f, 0.02f);
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { std::string(SynthParamIDs::noisePink), 1 }, "Pink Noise", false));
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { std::string(SynthParamIDs::mixerOd), 1 }, "Mixer OD", false));
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { std::string(SynthParamIDs::filterOd), 1 }, "Filter OD", false));
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { std::string(SynthParamIDs::oversample), 1 },
        "Oversampling",
        juce::StringArray { "Off", "2×", "4×" },
        0));
    addFloat(SynthParamIDs::mixerDrive, "Mixer Drive", 1.0f, 4.0f, 2.2f);
    addFloat(SynthParamIDs::subOscLevel, "Sub Osc", 0.0f, 1.0f, 0.0f);

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { std::string(SynthParamIDs::moogStack), 1 },
        "Moog Stack",
        juce::StringArray { "Voyager", "Sub Phatty", "Subsequent 37", "Model D" },
        0));

    addFloat(SynthParamIDs::filterCutoff, "Filter Cutoff", 20.0f, 18000.0f, 1800.0f, 0.3f);
    addFloat(SynthParamIDs::filterResonance, "Filter Resonance", 0.0f, 1.0f, 0.45f);
    addFloat(SynthParamIDs::filterDrive, "Filter Drive", 1.0f, 3.5f, 1.35f);
    addFloat(SynthParamIDs::filterEnvAmount, "Filter Env Amt", 0.0f, 1.0f, 0.7f);
    addFloat(SynthParamIDs::filterKeyTrack, "Filter KB Track", 0.0f, 1.0f, 0.35f);

    addFloat(SynthParamIDs::filterAttack, "Filter Attack", 0.001f, 4.0f, 0.005f, 0.35f);
    addFloat(SynthParamIDs::filterDecay, "Filter Decay", 0.001f, 4.0f, 0.35f, 0.35f);
    addFloat(SynthParamIDs::filterSustain, "Filter Sustain", 0.0f, 1.0f, 0.25f);
    addFloat(SynthParamIDs::filterRelease, "Filter Release", 0.001f, 6.0f, 0.5f, 0.35f);

    addFloat(SynthParamIDs::ampAttack, "Amp Attack", 0.001f, 4.0f, 0.002f, 0.35f);
    addFloat(SynthParamIDs::ampDecay, "Amp Decay", 0.001f, 4.0f, 0.25f, 0.35f);
    addFloat(SynthParamIDs::ampSustain, "Amp Sustain", 0.0f, 1.0f, 0.85f);
    addFloat(SynthParamIDs::ampRelease, "Amp Release", 0.001f, 8.0f, 0.45f, 0.35f);

    addFloat(SynthParamIDs::lfoRate, "LFO Rate", 0.05f, 20.0f, 0.6f, 0.35f);
    addFloat(SynthParamIDs::lfoToPitch, "LFO → Pitch", 0.0f, 1.0f, 0.0f);
    addFloat(SynthParamIDs::lfoToFilter, "LFO → Filter", 0.0f, 1.0f, 0.15f);
    addFloat(SynthParamIDs::glideTime, "Glide", 0.0f, 2.0f, 0.14f, 0.45f);

    return layout;
}

MoogVoyagerAudioProcessor::MoogVoyagerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
#endif
       apvts(*this, nullptr, "MoogVoyagerState", createParameterLayout())
{
    for (int i = 0; i < kNumVoices; ++i)
        synthesiser.addVoice(new SynthVoice(*this));
    synthesiser.addSound(new SynthSound());
    synthesiser.setNoteStealingEnabled(false);
    voyager::applyFactoryPreset(*this, 0);
}

MoogVoyagerAudioProcessor::~MoogVoyagerAudioProcessor() = default;

const juce::String MoogVoyagerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MoogVoyagerAudioProcessor::acceptsMidi() const { return true; }
bool MoogVoyagerAudioProcessor::producesMidi() const { return false; }
bool MoogVoyagerAudioProcessor::isMidiEffect() const { return false; }
double MoogVoyagerAudioProcessor::getTailLengthSeconds() const { return 3.0; }

int MoogVoyagerAudioProcessor::getNumPrograms() { return voyager::getFactoryPresetCount(); }
int MoogVoyagerAudioProcessor::getCurrentProgram() { return currentProgramIndex; }
void MoogVoyagerAudioProcessor::setCurrentProgram(int index)
{
    currentProgramIndex = juce::jlimit(0, getNumPrograms() - 1, index);
    voyager::applyFactoryPreset(*this, currentProgramIndex);
}
const juce::String MoogVoyagerAudioProcessor::getProgramName(int index)
{
    const auto name = voyager::getFactoryPresetName(index);
    return name.empty() ? juce::String() : juce::String(name.data());
}
void MoogVoyagerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void MoogVoyagerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synthesiser.setCurrentPlaybackSampleRate(sampleRate);
    const int osChoice = static_cast<int>(apvts.getRawParameterValue(std::string(SynthParamIDs::oversample))->load());
    oversampleFactor = osChoice == 1 ? 2 : (osChoice == 2 ? 4 : 1);
    const double maxSr = sampleRate * 4.0;
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<SynthVoice*>(synthesiser.getVoice(i)))
            voice->prepare(maxSr, samplesPerBlock * 4);

    oversampleScratch.setSize(getTotalNumOutputChannels(), samplesPerBlock * 4);

    outputGainLinear.reset(sampleRate, 0.02);
    outputGainLinear.setCurrentAndTargetValue(
        dbToLinear(apvts.getRawParameterValue(std::string(SynthParamIDs::outputGainDb))->load()));
}

void MoogVoyagerAudioProcessor::releaseResources() {}

bool MoogVoyagerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    return out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo();
}

void MoogVoyagerAudioProcessor::pushMeterScope(const juce::AudioBuffer<float>& buffer, int /*startSample*/,
                                               int numSamples) noexcept
{
    float peak = 0.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        float s = 0.0f;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            s += buffer.getSample(ch, i);
        s /= static_cast<float>(juce::jmax(1, buffer.getNumChannels()));
        peak = juce::jmax(peak, std::abs(s));
        const int idx = scopeWrite.fetch_add(1, std::memory_order_relaxed) & 255;
        scopeRing[static_cast<size_t>(idx)].store(s, std::memory_order_relaxed);
    }
    const float prev = outputPeakDb.load(std::memory_order_relaxed);
    const float db = peak > 1.0e-8f ? juce::Decibels::gainToDecibels(peak, -60.0f) : -60.0f;
  outputPeakDb.store(juce::jmax(db, prev * 0.92f), std::memory_order_relaxed);
}

void MoogVoyagerAudioProcessor::getScopeSnapshot(std::array<float, 256>& out) const noexcept
{
    const int end = scopeWrite.load(std::memory_order_relaxed);
    for (int i = 0; i < 256; ++i)
    {
        const int pos = end - 256 + i;
        const int idx = ((pos % 256) + 256) % 256;
        out[static_cast<size_t>(i)] = scopeRing[static_cast<size_t>(idx)].load(std::memory_order_relaxed);
    }
}

void MoogVoyagerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    cachedVoiceParams = readVoiceParams();

    const int osChoice = static_cast<int>(apvts.getRawParameterValue(std::string(SynthParamIDs::oversample))->load());
    oversampleFactor = osChoice == 1 ? 2 : (osChoice == 2 ? 4 : 1);

    outputGainLinear.setTargetValue(
        dbToLinear(apvts.getRawParameterValue(std::string(SynthParamIDs::outputGainDb))->load()));

    const double baseSr = getSampleRate();
    const int numSamples = buffer.getNumSamples();

    if (oversampleFactor <= 1)
    {
        synthesiser.setCurrentPlaybackSampleRate(baseSr);
        synthesiser.renderNextBlock(buffer, midi, 0, numSamples);
    }
    else
    {
        synthesiser.setCurrentPlaybackSampleRate(baseSr * static_cast<double>(oversampleFactor));
        const int osSamples = numSamples * oversampleFactor;
        oversampleScratch.setSize(buffer.getNumChannels(), osSamples, false, false, true);
        oversampleScratch.clear();
        synthesiser.renderNextBlock(oversampleScratch, midi, 0, osSamples);
        synthesiser.setCurrentPlaybackSampleRate(baseSr);

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            const float* src = oversampleScratch.getReadPointer(ch);
            float* dst = buffer.getWritePointer(ch);
            for (int i = 0; i < numSamples; ++i)
            {
                float sum = 0.0f;
                for (int k = 0; k < oversampleFactor; ++k)
                    sum += src[i * oversampleFactor + k];
                dst[i] = sum / static_cast<float>(oversampleFactor);
            }
        }
    }

    for (int i = 0; i < numSamples; ++i)
    {
        const float g = outputGainLinear.getNextValue();
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.setSample(ch, i, buffer.getSample(ch, i) * g);
    }

    pushMeterScope(buffer, 0, numSamples);
}

bool MoogVoyagerAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* MoogVoyagerAudioProcessor::createEditor()
{
    return new MoogVoyagerAudioProcessorEditor(*this);
}

void MoogVoyagerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty("factoryProgram", currentProgramIndex, nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MoogVoyagerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
    {
        auto state = juce::ValueTree::fromXml(*xml);
        apvts.replaceState(state);
        const int prog = static_cast<int>(state.getProperty("factoryProgram", 0));
        currentProgramIndex = juce::jlimit(0, getNumPrograms() - 1, prog);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MoogVoyagerAudioProcessor();
}
