#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Presets/PresetParams.h"

namespace
{
using PID = junovax::ParameterIDs;

void addFloat (juce::AudioProcessorValueTreeState::ParameterLayout& layout,
               const char* id,
               const juce::String& name,
               juce::NormalisableRange<float> range,
               float defaultValue)
{
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, 1 }, name, range, defaultValue));
}
} // namespace

juce::AudioProcessorValueTreeState::ParameterLayout JunovaXAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    addFloat (layout, PID::masterGain, "Master", { -24.0f, 12.0f, 0.01f }, 0.0f);

    addFloat (layout, PID::ampAttack, "Amp Attack", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.01f);
    addFloat (layout, PID::ampDecay, "Amp Decay", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.2f);
    addFloat (layout, PID::ampSustain, "Amp Sustain", { 0.0f, 1.0f, 0.001f }, 0.75f);
    addFloat (layout, PID::ampRelease, "Amp Release", { 0.001f, 5.0f, 0.001f, 0.35f }, 0.4f);

    addFloat (layout, PID::filtAttack, "Filt Attack", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.005f);
    addFloat (layout, PID::filtDecay, "Filt Decay", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.35f);
    addFloat (layout, PID::filtSustain, "Filt Sustain", { 0.0f, 1.0f, 0.001f }, 0.4f);
    addFloat (layout, PID::filtRelease, "Filt Release", { 0.001f, 5.0f, 0.001f, 0.35f }, 0.5f);

    addFloat (layout, PID::filterCutoff, "Cutoff", { 0.0f, 1.0f, 0.0001f }, 0.65f);
    addFloat (layout, PID::filterRes, "Resonance", { 0.0f, 1.0f, 0.0001f }, 0.15f);
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::hpfEnabled, 1 }, "HPF", false));

    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::chorusMode, 1 },
                                                              "Chorus",
                                                              juce::StringArray { "Off", "I", "II", "I+II" },
                                                              0));
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::voiceMode, 1 },
                                                              "Voice",
                                                              juce::StringArray { "Poly 1", "Poly 2", "Unison", "Mono" },
                                                              0));

    addFloat (layout, PID::lfoRate, "LFO Rate", { 0.01f, 20.0f, 0.001f, 0.4f }, 4.2f);
    addFloat (layout, PID::lfoDelay, "LFO Delay", { 0.0f, 2.0f, 0.001f }, 0.15f);
    addFloat (layout, PID::glide, "Glide", { 0.0f, 500.0f, 0.1f }, 12.0f);

    addFloat (layout, PID::dcoLfoMod, "DCO LFO Mod", { 0.0f, 100.0f, 0.1f }, 30.0f);
    addFloat (layout, PID::dcoPwm, "PWM", { 0.0f, 100.0f, 0.1f }, 65.0f);
    addFloat (layout, PID::dcoSubLvl, "Sub Level", { 0.0f, 100.0f, 0.1f }, 80.0f);
    addFloat (layout, PID::dcoNoise, "Noise", { 0.0f, 100.0f, 0.1f }, 10.0f);

    addFloat (layout, PID::hpfCutoff, "HPF Cutoff", { 0.0f, 1.0f, 0.0001f }, 0.2f);

    addFloat (layout, PID::vcfEnv, "VCF Env", { 0.0f, 100.0f, 0.1f }, 55.0f);
    addFloat (layout, PID::vcfLfo, "VCF LFO", { 0.0f, 100.0f, 0.1f }, 33.0f);
    addFloat (layout, PID::vcfKey, "VCF Key", { 0.0f, 100.0f, 0.1f }, 85.0f);

    addFloat (layout, PID::drift, "Drift", { 0.0f, 100.0f, 0.1f }, 14.0f);
    addFloat (layout, PID::detune, "Detune", { -50.0f, 50.0f, 0.01f }, 6.0f);
    addFloat (layout, PID::width, "Width", { 0.0f, 200.0f, 0.1f }, 100.0f);

    addFloat (layout, PID::arpRange, "Arp Range", { 1.0f, 4.0f, 1.0f }, 2.0f);
    addFloat (layout, PID::arpRate, "Arp Rate", { 0.0f, 1.0f, 0.001f }, 0.25f);
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::arpLatch, 1 }, "Arp Latch", false));

    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::diagTestTone, 1 }, "Diag Test Tone", false));
    addFloat (layout, PID::diagToneFreq, "Diag Freq", { 55.0f, 880.0f, 0.01f, 0.5f }, 440.0f);

    return layout;
}

JunovaXAudioProcessor::JunovaXAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
#endif
      apvts_ (*this, nullptr, "JunovaX", createParameterLayout())
{
    applyFactoryPreset (0);
}

JunovaXAudioProcessor::~JunovaXAudioProcessor() = default;

const juce::String JunovaXAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JunovaXAudioProcessor::acceptsMidi() const { return true; }
bool JunovaXAudioProcessor::producesMidi() const { return false; }
bool JunovaXAudioProcessor::isMidiEffect() const { return false; }
double JunovaXAudioProcessor::getTailLengthSeconds() const { return 0.5; }
bool JunovaXAudioProcessor::hasEditor() const { return true; }

int JunovaXAudioProcessor::getNumPrograms()
{
    return static_cast<int> (junovax::presets::getFactoryPresets().size());
}

int JunovaXAudioProcessor::getCurrentProgram() { return currentProgram_; }

void JunovaXAudioProcessor::setCurrentProgram (int index)
{
    applyFactoryPreset (index);
}

const juce::String JunovaXAudioProcessor::getProgramName (int index)
{
    const auto& presets = junovax::presets::getFactoryPresets();
    if (index >= 0 && index < static_cast<int> (presets.size()))
        return juce::String (presets[static_cast<std::size_t> (index)].name.data());
    return {};
}

void JunovaXAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void JunovaXAudioProcessor::applyFactoryPreset (int index)
{
    const auto& presets = junovax::presets::getFactoryPresets();
    if (index < 0 || index >= static_cast<int> (presets.size()))
        return;

    currentProgram_ = index;
    const auto& p = presets[static_cast<std::size_t> (index)].params;

    auto setF = [this] (const char* id, float v)
    {
        if (auto* param = apvts_.getParameter (id))
            param->setValueNotifyingHost (param->convertTo0to1 (v));
    };
    auto setB = [this] (const char* id, bool v)
    {
        if (auto* param = apvts_.getParameter (id))
            param->setValueNotifyingHost (v ? 1.0f : 0.0f);
    };
    auto setC = [this] (const char* id, int choiceIndex, int numChoices)
    {
        if (auto* param = apvts_.getParameter (id))
            param->setValueNotifyingHost (static_cast<float> (choiceIndex) / static_cast<float> (numChoices - 1));
    };

    setF (PID::masterGain, p.masterGain);
    setF (PID::ampAttack, p.ampAttack);
    setF (PID::ampDecay, p.ampDecay);
    setF (PID::ampSustain, p.ampSustain);
    setF (PID::ampRelease, p.ampRelease);
    setF (PID::filtAttack, p.filtAttack);
    setF (PID::filtDecay, p.filtDecay);
    setF (PID::filtSustain, p.filtSustain);
    setF (PID::filtRelease, p.filtRelease);
    setF (PID::filterCutoff, p.filterCutoff);
    setF (PID::filterRes, p.filterRes);
    setB (PID::hpfEnabled, p.hpfEnabled);
    setF (PID::hpfCutoff, p.hpfCutoff);
    setC (PID::chorusMode, p.chorusMode, 4);
    setC (PID::voiceMode, p.voiceMode, 4);
    setF (PID::lfoRate, p.lfoRate);
    setF (PID::lfoDelay, p.lfoDelay);
    setF (PID::glide, p.glide);
    setF (PID::dcoLfoMod, p.dcoLfoMod);
    setF (PID::dcoPwm, p.dcoPwm);
    setF (PID::dcoSubLvl, p.dcoSubLvl);
    setF (PID::dcoNoise, p.dcoNoise);
    setF (PID::vcfEnv, p.vcfEnv);
    setF (PID::vcfLfo, p.vcfLfo);
    setF (PID::vcfKey, p.vcfKey);
    setF (PID::drift, p.drift);
    setF (PID::detune, p.detune);
    setF (PID::width, p.width);
    setF (PID::arpRange, p.arpRange);
    setF (PID::arpRate, p.arpRate);
    setB (PID::arpLatch, p.arpLatch);

    pushParamsToEngine();
}

void JunovaXAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lastSampleRate_ = sampleRate;
    arpeggiator_.prepare (sampleRate);
    engine_.prepare (sampleRate, samplesPerBlock);
    pushParamsToEngine();
}

void JunovaXAudioProcessor::releaseResources() {}

bool JunovaXAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void JunovaXAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    pushParamsToEngine();

    junovax::dsp::ArpHostContext host;
    host.bpm = 120.0;
    host.ppqValid = false;
    if (auto* head = getPlayHead())
    {
        if (auto pos = head->getPosition())
        {
            host.bpm = pos->getBpm().orFallback (120.0);
            if (auto ppq = pos->getPpqPosition())
            {
                host.ppqPosition = *ppq;
                host.ppqValid = true;
            }
        }
    }
    displayBpm_.store (static_cast<float> (host.bpm), std::memory_order_relaxed);

    const auto params = readParamsFromApvts();
    juce::MidiBuffer arpMidi;
    arpeggiator_.process (midi,
                          arpMidi,
                          params.arpRate,
                          params.arpRange,
                          params.arpLatch,
                          host,
                          buffer.getNumSamples());
    engine_.render (buffer, arpMidi);

    if (buffer.getNumChannels() > 0)
    {
        const auto* mono = buffer.getReadPointer (0);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            scopeFifo_.pushSample (mono[i]);
    }
}

void JunovaXAudioProcessor::panicAllNotes()
{
    arpeggiator_.reset();
    engine_.panic();
    if (auto* tone = apvts_.getParameter (PID::diagTestTone))
        tone->setValueNotifyingHost (0.0f);
}

junovax::dsp::RuntimeParams JunovaXAudioProcessor::readParamsFromApvts() const noexcept
{
    junovax::dsp::RuntimeParams p;
    auto f = [this] (const char* id) -> float
    {
        if (auto* param = apvts_.getRawParameterValue (id))
            return param->load();
        return 0.0f;
    };

    p.masterGainDb = f (PID::masterGain);
    p.ampAttack = f (PID::ampAttack);
    p.ampDecay = f (PID::ampDecay);
    p.ampSustain = f (PID::ampSustain);
    p.ampRelease = f (PID::ampRelease);
    p.filtAttack = f (PID::filtAttack);
    p.filtDecay = f (PID::filtDecay);
    p.filtSustain = f (PID::filtSustain);
    p.filtRelease = f (PID::filtRelease);
    p.filterCutoff = f (PID::filterCutoff);
    p.filterRes = f (PID::filterRes);
    p.hpfEnabled = f (PID::hpfEnabled) > 0.5f;
    p.hpfCutoff = f (PID::hpfCutoff);
    if (auto* chorus = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (PID::chorusMode)))
        p.chorusMode = chorus->getIndex();
    if (auto* voice = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (PID::voiceMode)))
        p.voiceMode = voice->getIndex();
    p.lfoRate = f (PID::lfoRate);
    p.lfoDelay = f (PID::lfoDelay);
    p.glideMs = f (PID::glide);
    p.dcoLfoMod = f (PID::dcoLfoMod);
    p.dcoPwm = f (PID::dcoPwm);
    p.dcoSubLvl = f (PID::dcoSubLvl);
    p.dcoNoise = f (PID::dcoNoise);
    p.vcfEnv = f (PID::vcfEnv);
    p.vcfLfo = f (PID::vcfLfo);
    p.vcfKey = f (PID::vcfKey);
    p.drift = f (PID::drift);
    p.detune = f (PID::detune);
    p.width = f (PID::width);
    p.arpRange = f (PID::arpRange);
    p.arpRate = f (PID::arpRate);
    p.arpLatch = f (PID::arpLatch) > 0.5f;
    p.diagTestTone = f (PID::diagTestTone) > 0.5f;
    p.diagToneFreqHz = f (PID::diagToneFreq);
    return p;
}

void JunovaXAudioProcessor::pushParamsToEngine() noexcept
{
    engine_.setParams (readParamsFromApvts());
}

void JunovaXAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    state.setProperty ("currentProgram", currentProgram_, nullptr);
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void JunovaXAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
    {
        if (xml->hasTagName (apvts_.state.getType()))
        {
            auto tree = juce::ValueTree::fromXml (*xml);
            apvts_.replaceState (tree);
            const int prog = static_cast<int> (tree.getProperty ("currentProgram", 0));
            currentProgram_ = prog;
            pushParamsToEngine();
        }
    }
}

juce::AudioProcessorEditor* JunovaXAudioProcessor::createEditor()
{
    return new JunovaXAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JunovaXAudioProcessor();
}
