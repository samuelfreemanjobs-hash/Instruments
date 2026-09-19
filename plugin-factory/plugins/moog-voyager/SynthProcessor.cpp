#include "SynthProcessor.h"
#include "SynthEditor.h"
#include "GfunkPresets.h"

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

float MoogVoyagerAudioProcessor::SynthVoice::nextNoiseSample() noexcept
{
    rngState = rngState * 1664525u + 1013904223u;
    return (static_cast<int32_t>(rngState) / static_cast<float>(INT32_MAX)) * 2.0f - 1.0f;
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
            const float drift = 1.0f + 0.0012f * std::sin(driftPhase[o] * kTwoPi);
            const float hz = rootHz * std::pow(2.0f, params.oscOctave[o])
                             * std::pow(2.0f, params.oscFine[o] / 1200.0f) * drift;
            const float inc = hz / sr;
            phase[o] += inc;
            if (phase[o] >= 1.0f)
                phase[o] -= 1.0f;
            mixed += waveFromPhase(phase[o], inc, params.oscWave[o]) * params.oscLevel[o];
        }
        mixed += nextNoiseSample() * params.noiseLevel;
        mixed = mixerSaturate(mixed, params.mixerDrive);

        const float filterEnv = filterAdsr.getNextSample();
        const float ampEnv = ampAdsr.getNextSample();

        const float cutoff = computeCutoffHz(params, filterEnv, lfo, currentMidiNote);

        ladder.setCutoffFrequencyHz(cutoff);
        ladder.setResonance(params.filterResonance);
        ladder.setDrive(params.filterDrive);

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
    addFloat(SynthParamIDs::mixerDrive, "Mixer Drive", 1.0f, 4.0f, 2.2f);

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
    gfunk::applyPreset(*this, gfunk::PresetId::gFunkLead);
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

int MoogVoyagerAudioProcessor::getNumPrograms() { return 2; }
int MoogVoyagerAudioProcessor::getCurrentProgram() { return currentProgramIndex; }
void MoogVoyagerAudioProcessor::setCurrentProgram(int index)
{
    currentProgramIndex = juce::jlimit(0, getNumPrograms() - 1, index);
    gfunk::applyPreset(*this,
                       currentProgramIndex == 0 ? gfunk::PresetId::gFunkLead : gfunk::PresetId::gFunkBass);
}
const juce::String MoogVoyagerAudioProcessor::getProgramName(int index)
{
    if (index == 0)
        return "G-Funk Lead";
    if (index == 1)
        return "G-Funk Bass";
    return {};
}
void MoogVoyagerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void MoogVoyagerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synthesiser.setCurrentPlaybackSampleRate(sampleRate);
    for (int i = 0; i < synthesiser.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<SynthVoice*>(synthesiser.getVoice(i)))
            voice->prepare(sampleRate, samplesPerBlock);

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

void MoogVoyagerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    cachedVoiceParams = readVoiceParams();

    outputGainLinear.setTargetValue(
        dbToLinear(apvts.getRawParameterValue(std::string(SynthParamIDs::outputGainDb))->load()));

    synthesiser.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        const float g = outputGainLinear.getNextValue();
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.setSample(ch, i, buffer.getSample(ch, i) * g);
    }
}

bool MoogVoyagerAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* MoogVoyagerAudioProcessor::createEditor()
{
    return new MoogVoyagerAudioProcessorEditor(*this);
}

void MoogVoyagerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MoogVoyagerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MoogVoyagerAudioProcessor();
}
