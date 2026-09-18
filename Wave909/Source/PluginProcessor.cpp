#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
using PID = wave909::ParameterIDs;

void addFloatParam (juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                    const char* id,
                    const juce::String& name,
                    juce::NormalisableRange<float> range,
                    float defaultValue)
{
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, 1 }, name, range, defaultValue));
}
} // namespace

juce::AudioProcessorValueTreeState::ParameterLayout Wave909AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    addFloatParam (layout, PID::outputGain, "Output", { -24.0f, 12.0f, 0.01f }, -3.0f);
    addFloatParam (layout, PID::wtPosition, "Wavetable", { 0.0f, 1.0f, 0.0001f }, 0.35f);
    addFloatParam (layout, PID::wtScan, "WT Scan", { 0.0f, 1.0f, 0.0001f }, 0.0f);
    addFloatParam (layout, PID::filterCutoff, "Cutoff", { 0.0f, 1.0f, 0.0001f }, 0.72f);
    addFloatParam (layout, PID::filterRes, "Resonance", { 0.0f, 1.0f, 0.0001f }, 0.25f);
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::filterCircuit, 1 }, "Destructive Circuit", false));
    addFloatParam (layout, PID::drive, "Drive", { 0.0f, 1.0f, 0.0001f }, 0.2f);
    addFloatParam (layout, PID::crush, "Crush", { 0.0f, 1.0f, 0.0001f }, 0.0f);
    addFloatParam (layout, PID::subLevel, "Sub", { 0.0f, 1.0f, 0.0001f }, 0.35f);
    addFloatParam (layout, PID::detune, "Detune", { -50.0f, 50.0f, 0.01f }, 0.0f);
    addFloatParam (layout, PID::glide, "Glide", { 0.0f, 500.0f, 0.1f }, 0.0f);
    addFloatParam (layout, PID::tapeWobble, "Tape Wobble", { 0.0f, 1.0f, 0.0001f }, 0.0f);

    addFloatParam (layout, PID::ampAttack, "Amp A", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.01f);
    addFloatParam (layout, PID::ampDecay, "Amp D", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.15f);
    addFloatParam (layout, PID::ampSustain, "Amp S", { 0.0f, 1.0f, 0.001f }, 0.85f);
    addFloatParam (layout, PID::ampRelease, "Amp R", { 0.001f, 5.0f, 0.001f, 0.35f }, 0.35f);
    addFloatParam (layout, PID::filtAttack, "Filt A", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.005f);
    addFloatParam (layout, PID::filtDecay, "Filt D", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.4f);
    addFloatParam (layout, PID::filtSustain, "Filt S", { 0.0f, 1.0f, 0.001f }, 0.3f);
    addFloatParam (layout, PID::filtRelease, "Filt R", { 0.001f, 5.0f, 0.001f, 0.35f }, 0.5f);
    addFloatParam (layout, PID::filtEnvAmt, "Filt Env", { 0.0f, 1.0f, 0.001f }, 0.55f);
    addFloatParam (layout, PID::stereoWidth, "Width", { 0.0f, 1.0f, 0.001f }, 0.15f);

    return layout;
}

Wave909AudioProcessor::Wave909AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
#endif
      apvts_ (*this, nullptr, "WAVE-909", createParameterLayout())
{
    applyFactoryPreset (0);
}

Wave909AudioProcessor::~Wave909AudioProcessor() = default;

const juce::String Wave909AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Wave909AudioProcessor::acceptsMidi() const { return true; }
bool Wave909AudioProcessor::producesMidi() const { return false; }
bool Wave909AudioProcessor::isMidiEffect() const { return false; }
double Wave909AudioProcessor::getTailLengthSeconds() const { return 2.0; }
bool Wave909AudioProcessor::hasEditor() const { return true; }

int Wave909AudioProcessor::getNumPrograms()
{
    return static_cast<int> (wave909::presets::getFactoryPresets().size());
}

int Wave909AudioProcessor::getCurrentProgram() { return currentProgram_; }

void Wave909AudioProcessor::setCurrentProgram (int index)
{
    applyFactoryPreset (index);
}

const juce::String Wave909AudioProcessor::getProgramName (int index)
{
    const auto& presets = wave909::presets::getFactoryPresets();
    if (index >= 0 && index < static_cast<int> (presets.size()))
        return juce::String (presets[static_cast<std::size_t> (index)].name.data());
    return {};
}

void Wave909AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void Wave909AudioProcessor::applyFactoryPreset (int index)
{
    const auto& presets = wave909::presets::getFactoryPresets();
    if (index < 0 || index >= static_cast<int> (presets.size()))
        return;

    currentProgram_ = index;
    const auto& preset = presets[static_cast<std::size_t> (index)];
    const auto& p = preset.params;

    auto set = [this] (const char* id, float v) { if (auto* param = apvts_.getParameter (id)) param->setValueNotifyingHost (param->convertTo0to1 (v)); };

    set (PID::wtPosition, p.wtPosition);
    set (PID::wtScan, p.wtScan);
    set (PID::filterCutoff, p.filterCutoff);
    set (PID::filterRes, p.filterRes);
    if (auto* circuit = apvts_.getParameter (PID::filterCircuit))
        circuit->setValueNotifyingHost (p.filterDestructive ? 1.0f : 0.0f);
    set (PID::drive, p.drive);
    set (PID::crush, p.crush);
    set (PID::subLevel, p.subLevel);
    set (PID::detune, p.detuneCents);
    set (PID::glide, p.glideMs);
    set (PID::tapeWobble, p.tapeWobble);
    set (PID::ampAttack, p.ampAttack);
    set (PID::ampDecay, p.ampDecay);
    set (PID::ampSustain, p.ampSustain);
    set (PID::ampRelease, p.ampRelease);
    set (PID::filtAttack, p.filtAttack);
    set (PID::filtDecay, p.filtDecay);
    set (PID::filtSustain, p.filtSustain);
    set (PID::filtRelease, p.filtRelease);
    set (PID::filtEnvAmt, p.filtEnvAmt);
    set (PID::stereoWidth, p.stereoWidth);

    refreshParameters();
    pushParamsToEngine();
}

void Wave909AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    engine_.prepare (sampleRate, samplesPerBlock);
    refreshParameters();
    pushParamsToEngine();
}

void Wave909AudioProcessor::releaseResources()
{
    engine_.reset();
}

bool Wave909AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& main = layouts.getMainOutputChannelSet();
    return main == juce::AudioChannelSet::mono() || main == juce::AudioChannelSet::stereo();
}

wave909::SynthParams Wave909AudioProcessor::readParamsFromApvts() const noexcept
{
    auto load = [this] (const char* id) -> float
    {
        if (auto* p = apvts_.getRawParameterValue (id))
            return p->load();
        return 0.0f;
    };

    wave909::SynthParams p;
    p.outputGainDb = load (PID::outputGain);
    p.wtPosition = load (PID::wtPosition);
    p.wtScan = load (PID::wtScan);
    p.filterCutoff = load (PID::filterCutoff);
    p.filterRes = load (PID::filterRes);
    p.filterDestructive = load (PID::filterCircuit) > 0.5f;
    p.drive = load (PID::drive);
    p.crush = load (PID::crush);
    p.subLevel = load (PID::subLevel);
    p.detuneCents = load (PID::detune);
    p.glideMs = load (PID::glide);
    p.tapeWobble = load (PID::tapeWobble);
    p.ampAttack = load (PID::ampAttack);
    p.ampDecay = load (PID::ampDecay);
    p.ampSustain = load (PID::ampSustain);
    p.ampRelease = load (PID::ampRelease);
    p.filtAttack = load (PID::filtAttack);
    p.filtDecay = load (PID::filtDecay);
    p.filtSustain = load (PID::filtSustain);
    p.filtRelease = load (PID::filtRelease);
    p.filtEnvAmt = load (PID::filtEnvAmt);
    p.stereoWidth = load (PID::stereoWidth);
    return p;
}

void Wave909AudioProcessor::refreshParameters() noexcept
{
    liveParams_ = readParamsFromApvts();
}

void Wave909AudioProcessor::pushParamsToEngine() noexcept
{
    engine_.setParams (liveParams_);
}

void Wave909AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
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

    const float gain = juce::Decibels::decibelsToGain (liveParams_.outputGainDb, -100.0f);
    buffer.applyGain (gain);

    for (int i = 0; i < numSamples; ++i)
    {
        left[i] = std::tanh (left[i]);
        if (buffer.getNumChannels() > 1)
            right[i] = std::tanh (right[i]);
    }
}

juce::AudioProcessorEditor* Wave909AudioProcessor::createEditor()
{
    return new Wave909AudioProcessorEditor (*this);
}

void Wave909AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    state.setProperty ("currentProgram", currentProgram_, nullptr);
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void Wave909AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new Wave909AudioProcessor();
}
