#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
using PID = pluggva::ParameterIDs;

int readChoiceIndex (const juce::AudioProcessorValueTreeState& apvts, const char* id) noexcept
{
    if (auto* param = apvts.getParameter (id))
        if (auto* choice = dynamic_cast<juce::AudioParameterChoice*> (param))
            return choice->getIndex();
    return 0;
}

void addFloatParam (juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                    const char* id,
                    const juce::String& name,
                    juce::NormalisableRange<float> range,
                    float defaultValue)
{
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, 1 }, name, range, defaultValue));
}
} // namespace

juce::AudioProcessorValueTreeState::ParameterLayout PluggVAAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    addFloatParam (layout, PID::outputGain, "Output", { -24.0f, 12.0f, 0.01f }, -3.0f);

    const juce::StringArray waves { "Saw", "Square", "Pulse", "Triangle" };
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::osc1Wave, 1 }, "OSC1 Wave", waves, 0));
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::osc2Wave, 1 }, "OSC2 Wave", waves, 0));

    addFloatParam (layout, PID::osc1Level, "OSC1", { 0.0f, 1.0f, 0.001f }, 0.85f);
    addFloatParam (layout, PID::osc2Level, "OSC2", { 0.0f, 1.0f, 0.001f }, 0.55f);
    addFloatParam (layout, PID::osc2Detune, "OSC2 Detune", { -50.0f, 50.0f, 0.01f }, 7.0f);

    const juce::StringArray octaves { "-1 Oct", "0 Oct", "+1 Oct" };
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::osc2Octave, 1 }, "OSC2 Oct", octaves, 1));

    addFloatParam (layout, PID::pulseWidth, "Pulse W", { 0.05f, 0.95f, 0.001f }, 0.42f);
    addFloatParam (layout, PID::subLevel, "Sub", { 0.0f, 1.0f, 0.001f }, 0.35f);
    addFloatParam (layout, PID::unison, "Unison", { 0.0f, 1.0f, 0.001f }, 0.35f);
    addFloatParam (layout, PID::unisonSpread, "Spread", { 0.0f, 50.0f, 0.1f }, 18.0f);

    addFloatParam (layout, PID::filterCutoff, "Cutoff", { 0.0f, 1.0f, 0.0001f }, 0.68f);
    addFloatParam (layout, PID::filterRes, "Res", { 0.0f, 1.0f, 0.0001f }, 0.22f);
    addFloatParam (layout, PID::filtEnvAmt, "F.Env", { 0.0f, 1.0f, 0.001f }, 0.5f);
    addFloatParam (layout, PID::drive, "Drive", { 0.0f, 1.0f, 0.001f }, 0.15f);
    addFloatParam (layout, PID::crush, "Crush", { 0.0f, 1.0f, 0.001f }, 0.0f);
    addFloatParam (layout, PID::chorus, "Chorus", { 0.0f, 1.0f, 0.001f }, 0.2f);
    addFloatParam (layout, PID::drift, "Drift", { 0.0f, 1.0f, 0.001f }, 0.0f);
    addFloatParam (layout, PID::glide, "Glide", { 0.0f, 500.0f, 0.1f }, 0.0f);
    addFloatParam (layout, PID::stereoWidth, "Width", { 0.0f, 1.0f, 0.001f }, 0.2f);

    addFloatParam (layout, PID::ampAttack, "Amp A", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.01f);
    addFloatParam (layout, PID::ampDecay, "Amp D", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.15f);
    addFloatParam (layout, PID::ampSustain, "Amp S", { 0.0f, 1.0f, 0.001f }, 0.85f);
    addFloatParam (layout, PID::ampRelease, "Amp R", { 0.001f, 5.0f, 0.001f, 0.35f }, 0.35f);
    addFloatParam (layout, PID::filtAttack, "Filt A", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.005f);
    addFloatParam (layout, PID::filtDecay, "Filt D", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.4f);
    addFloatParam (layout, PID::filtSustain, "Filt S", { 0.0f, 1.0f, 0.001f }, 0.3f);
    addFloatParam (layout, PID::filtRelease, "Filt R", { 0.001f, 5.0f, 0.001f, 0.35f }, 0.5f);

    return layout;
}

PluggVAAudioProcessor::PluggVAAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
#endif
      apvts_ (*this, nullptr, "PluggVA", createParameterLayout())
{
    applyFactoryPreset (0);
}

PluggVAAudioProcessor::~PluggVAAudioProcessor() = default;

const juce::String PluggVAAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluggVAAudioProcessor::acceptsMidi() const { return true; }
bool PluggVAAudioProcessor::producesMidi() const { return false; }
bool PluggVAAudioProcessor::isMidiEffect() const { return false; }
double PluggVAAudioProcessor::getTailLengthSeconds() const { return 2.0; }
bool PluggVAAudioProcessor::hasEditor() const { return true; }

int PluggVAAudioProcessor::getNumPrograms()
{
    return static_cast<int> (pluggva::presets::getFactoryPresets().size());
}

int PluggVAAudioProcessor::getCurrentProgram() { return currentProgram_; }

void PluggVAAudioProcessor::setCurrentProgram (int index)
{
    applyFactoryPreset (index);
}

const juce::String PluggVAAudioProcessor::getProgramName (int index)
{
    const auto& presets = pluggva::presets::getFactoryPresets();
    if (index >= 0 && index < static_cast<int> (presets.size()))
        return juce::String (presets[static_cast<std::size_t> (index)].name);
    return {};
}

void PluggVAAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void PluggVAAudioProcessor::applyFactoryPreset (int index)
{
    const auto& presets = pluggva::presets::getFactoryPresets();
    if (index < 0 || index >= static_cast<int> (presets.size()))
        return;

    currentProgram_ = index;
    const auto& preset = presets[static_cast<std::size_t> (index)];
    const auto& p = preset.params;

    auto setF = [this] (const char* id, float v)
    {
        if (auto* param = apvts_.getParameter (id))
            param->setValueNotifyingHost (param->convertTo0to1 (v));
    };

    auto setChoice = [this] (const char* id, int idx)
    {
        if (auto* param = apvts_.getParameter (id))
            param->setValueNotifyingHost (param->convertTo0to1 (static_cast<float> (idx)));
    };

    setF (PID::outputGain, p.outputGainDb);
    setChoice (PID::osc1Wave, static_cast<int> (p.osc1Wave));
    setChoice (PID::osc2Wave, static_cast<int> (p.osc2Wave));
    setF (PID::osc1Level, p.osc1Level);
    setF (PID::osc2Level, p.osc2Level);
    setF (PID::osc2Detune, p.osc2DetuneCents);
    setChoice (PID::osc2Octave, p.osc2Octave + 1);
    setF (PID::pulseWidth, p.pulseWidth);
    setF (PID::subLevel, p.subLevel);
    setF (PID::unison, p.unisonAmount);
    setF (PID::unisonSpread, p.unisonSpreadCents);
    setF (PID::filterCutoff, p.filterCutoff);
    setF (PID::filterRes, p.filterRes);
    setF (PID::filtEnvAmt, p.filtEnvAmt);
    setF (PID::drive, p.drive);
    setF (PID::crush, p.crush);
    setF (PID::chorus, p.chorus);
    setF (PID::drift, p.drift);
    setF (PID::glide, p.glideMs);
    setF (PID::stereoWidth, p.stereoWidth);
    setF (PID::ampAttack, p.ampAttack);
    setF (PID::ampDecay, p.ampDecay);
    setF (PID::ampSustain, p.ampSustain);
    setF (PID::ampRelease, p.ampRelease);
    setF (PID::filtAttack, p.filtAttack);
    setF (PID::filtDecay, p.filtDecay);
    setF (PID::filtSustain, p.filtSustain);
    setF (PID::filtRelease, p.filtRelease);

    refreshParameters();
    pushParamsToEngine();
    chorus_.setAmount (liveParams_.chorus);
}

void PluggVAAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    engine_.prepare (sampleRate, samplesPerBlock);
    chorus_.prepare (sampleRate);
    refreshParameters();
    pushParamsToEngine();
    chorus_.setAmount (liveParams_.chorus);
}

void PluggVAAudioProcessor::releaseResources()
{
    engine_.reset();
}

bool PluggVAAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& main = layouts.getMainOutputChannelSet();
    return main == juce::AudioChannelSet::mono() || main == juce::AudioChannelSet::stereo();
}

pluggva::SynthParams PluggVAAudioProcessor::readParamsFromApvts() const noexcept
{
    auto load = [this] (const char* id) -> float
    {
        if (auto* p = apvts_.getRawParameterValue (id))
            return p->load();
        return 0.0f;
    };

    pluggva::SynthParams p;
    p.outputGainDb = load (PID::outputGain);
    p.osc1Wave = static_cast<pluggva::Waveform> (std::clamp (readChoiceIndex (apvts_, PID::osc1Wave), 0, 3));
    p.osc2Wave = static_cast<pluggva::Waveform> (std::clamp (readChoiceIndex (apvts_, PID::osc2Wave), 0, 3));
    p.osc1Level = load (PID::osc1Level);
    p.osc2Level = load (PID::osc2Level);
    p.osc2DetuneCents = load (PID::osc2Detune);
    p.osc2Octave = readChoiceIndex (apvts_, PID::osc2Octave) - 1;
    p.pulseWidth = load (PID::pulseWidth);
    p.subLevel = load (PID::subLevel);
    p.unisonAmount = load (PID::unison);
    p.unisonSpreadCents = load (PID::unisonSpread);
    p.filterCutoff = load (PID::filterCutoff);
    p.filterRes = load (PID::filterRes);
    p.filtEnvAmt = load (PID::filtEnvAmt);
    p.drive = load (PID::drive);
    p.crush = load (PID::crush);
    p.chorus = load (PID::chorus);
    p.drift = load (PID::drift);
    p.glideMs = load (PID::glide);
    p.stereoWidth = load (PID::stereoWidth);
    p.ampAttack = load (PID::ampAttack);
    p.ampDecay = load (PID::ampDecay);
    p.ampSustain = load (PID::ampSustain);
    p.ampRelease = load (PID::ampRelease);
    p.filtAttack = load (PID::filtAttack);
    p.filtDecay = load (PID::filtDecay);
    p.filtSustain = load (PID::filtSustain);
    p.filtRelease = load (PID::filtRelease);
    return p;
}

void PluggVAAudioProcessor::refreshParameters() noexcept
{
    liveParams_ = readParamsFromApvts();
}

void PluggVAAudioProcessor::pushParamsToEngine() noexcept
{
    engine_.setParams (liveParams_);
    chorus_.setAmount (liveParams_.chorus);
}

void PluggVAAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    refreshParameters();
    pushParamsToEngine();

    const int numSamples = buffer.getNumSamples();
    auto* left = buffer.getWritePointer (0);
    auto* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : left;

    buffer.clear();

    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
            engine_.noteOn (msg.getNoteNumber(), msg.getFloatVelocity());
        else if (msg.isNoteOff())
            engine_.noteOff (msg.getNoteNumber());
    }

    engine_.render (left, right, numSamples);
    chorus_.processBlock (left, right, numSamples);

    const float gain = juce::Decibels::decibelsToGain (liveParams_.outputGainDb, -100.0f);
    buffer.applyGain (gain);

    for (int i = 0; i < numSamples; ++i)
    {
        left[i] = std::tanh (left[i]);
        if (buffer.getNumChannels() > 1)
            right[i] = std::tanh (right[i]);
    }
}

juce::AudioProcessorEditor* PluggVAAudioProcessor::createEditor()
{
    return new PluggVAAudioProcessorEditor (*this);
}

void PluggVAAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    state.setProperty ("currentProgram", currentProgram_, nullptr);
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PluggVAAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (apvts_.state.getType()))
    {
        auto vt = juce::ValueTree::fromXml (*xml);
        currentProgram_ = static_cast<int> (vt.getProperty ("currentProgram", 0));
        apvts_.replaceState (vt);
        refreshParameters();
        pushParamsToEngine();
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluggVAAudioProcessor();
}
