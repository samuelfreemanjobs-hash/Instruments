#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "BinaryData.h"

namespace
{
using PID = disklordz::rompler::ParameterIDs;

void addFloat (juce::AudioProcessorValueTreeState::ParameterLayout& layout,
               const char* id,
               const juce::String& name,
               juce::NormalisableRange<float> range,
               float def)
{
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, 1 }, name, range, def));
}
} // namespace

juce::AudioProcessorValueTreeState::ParameterLayout DisklordzRomplerProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    addFloat (layout, PID::outputGain, "Output", { -24.0f, 6.0f, 0.01f }, -3.0f);
    addFloat (layout, PID::tone1, "Tone 1", { 0.0f, 1.0f, 0.001f }, 0.85f);
    addFloat (layout, PID::tone2, "Tone 2", { 0.0f, 1.0f, 0.001f }, 0.55f);
    addFloat (layout, PID::tone3, "Tone 3", { 0.0f, 1.0f, 0.001f }, 0.35f);
    addFloat (layout, PID::tone4, "Tone 4", { 0.0f, 1.0f, 0.001f }, 0.75f);
    addFloat (layout, PID::macroCharacter, "Character", { 0.0f, 1.0f, 0.001f }, 0.5f);
    addFloat (layout, PID::macroBrightness, "Brightness", { 0.0f, 1.0f, 0.001f }, 0.45f);
    addFloat (layout, PID::macroDrive, "Drive", { 0.0f, 1.0f, 0.001f }, 0.35f);
    addFloat (layout, PID::macroSpace, "Space", { 0.0f, 1.0f, 0.001f }, 0.4f);
    addFloat (layout, PID::macroMovement, "Movement", { 0.0f, 1.0f, 0.001f }, 0.3f);
    addFloat (layout, PID::macroWidth, "Width", { 0.0f, 1.0f, 0.001f }, 0.5f);
    addFloat (layout, PID::macroDecay, "Decay", { 0.0f, 1.0f, 0.001f }, 0.4f);
    addFloat (layout, PID::macroCrush, "Crush", { 0.0f, 1.0f, 0.001f }, 0.2f);
    addFloat (layout, PID::filterCutoff, "Cutoff", { 0.0f, 1.0f, 0.001f }, 0.75f);
    addFloat (layout, PID::ampAttack, "Amp A", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.005f);
    addFloat (layout, PID::ampDecay, "Amp D", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.25f);
    addFloat (layout, PID::ampSustain, "Amp S", { 0.0f, 1.0f, 0.001f }, 0.85f);
    addFloat (layout, PID::ampRelease, "Amp R", { 0.001f, 5.0f, 0.001f, 0.35f }, 0.35f);
    return layout;
}

DisklordzRomplerProcessor::DisklordzRomplerProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
#endif
      apvts_ (*this, nullptr, "DisklordzRompler", createParameterLayout())
{
    romBank_.loadEmbedded (BinaryData::disklordz_factory_dlrrom,
                           static_cast<std::size_t> (BinaryData::disklordz_factory_dlrromSize));
    engine_.setRomBank (&romBank_);
    applyFactoryPreset (0);
}

DisklordzRomplerProcessor::~DisklordzRomplerProcessor() = default;

const juce::String DisklordzRomplerProcessor::getName() const { return JucePlugin_Name; }
bool DisklordzRomplerProcessor::acceptsMidi() const { return true; }
bool DisklordzRomplerProcessor::producesMidi() const { return false; }
bool DisklordzRomplerProcessor::isMidiEffect() const { return false; }
double DisklordzRomplerProcessor::getTailLengthSeconds() const { return 2.5; }
bool DisklordzRomplerProcessor::hasEditor() const { return true; }

int DisklordzRomplerProcessor::getNumPrograms()
{
    return static_cast<int> (disklordz::rompler::presets::getFactoryPresets().size());
}

int DisklordzRomplerProcessor::getCurrentProgram() { return currentProgram_; }

void DisklordzRomplerProcessor::setCurrentProgram (int index)
{
    applyFactoryPreset (index);
}

const juce::String DisklordzRomplerProcessor::getProgramName (int index)
{
    const auto& presets = disklordz::rompler::presets::getFactoryPresets();
    if (index >= 0 && index < static_cast<int> (presets.size()))
        return juce::String (presets[static_cast<std::size_t> (index)].name.data());
    return {};
}

void DisklordzRomplerProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void DisklordzRomplerProcessor::applyFactoryPreset (int index)
{
    const auto& presets = disklordz::rompler::presets::getFactoryPresets();
    if (index < 0 || index >= static_cast<int> (presets.size()))
        return;

    currentProgram_ = index;
    const auto& preset = presets[static_cast<std::size_t> (index)];
    const auto& p = preset.params;

    auto set = [this] (const char* id, float v)
    {
        if (auto* param = apvts_.getParameter (id))
            param->setValueNotifyingHost (param->convertTo0to1 (v));
    };

    set (PID::tone1, p.toneLevel[0]);
    set (PID::tone2, p.toneLevel[1]);
    set (PID::tone3, p.toneLevel[2]);
    set (PID::tone4, p.toneLevel[3]);
    set (PID::macroCharacter, p.macroCharacter);
    set (PID::macroBrightness, p.macroBrightness);
    set (PID::macroDrive, p.macroDrive);
    set (PID::macroSpace, p.macroSpace);
    set (PID::macroMovement, p.macroMovement);
    set (PID::macroWidth, p.macroWidth);
    set (PID::macroDecay, p.macroDecay);
    set (PID::macroCrush, p.macroCrush);
    set (PID::filterCutoff, p.filterCutoff);
    set (PID::ampAttack, p.ampAttack);
    set (PID::ampDecay, p.ampDecay);
    set (PID::ampSustain, p.ampSustain);
    set (PID::ampRelease, p.ampRelease);

    refreshParameters();
    pushParamsToEngine();
}

void DisklordzRomplerProcessor::prepareToPlay (double sampleRate, int)
{
    engine_.prepare (sampleRate);
    refreshParameters();
    pushParamsToEngine();
}

void DisklordzRomplerProcessor::releaseResources()
{
    engine_.reset();
}

bool DisklordzRomplerProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& main = layouts.getMainOutputChannelSet();
    return main == juce::AudioChannelSet::mono() || main == juce::AudioChannelSet::stereo();
}

disklordz::rompler::RomplerParams DisklordzRomplerProcessor::readParamsFromApvts() const noexcept
{
    auto load = [this] (const char* id) -> float
    {
        if (auto* p = apvts_.getRawParameterValue (id))
            return p->load();
        return 0.0f;
    };

    disklordz::rompler::RomplerParams p;
    p.outputGainDb = load (PID::outputGain);
    p.toneLevel[0] = load (PID::tone1);
    p.toneLevel[1] = load (PID::tone2);
    p.toneLevel[2] = load (PID::tone3);
    p.toneLevel[3] = load (PID::tone4);
    p.macroCharacter = load (PID::macroCharacter);
    p.macroBrightness = load (PID::macroBrightness);
    p.macroDrive = load (PID::macroDrive);
    p.macroSpace = load (PID::macroSpace);
    p.macroMovement = load (PID::macroMovement);
    p.macroWidth = load (PID::macroWidth);
    p.macroDecay = load (PID::macroDecay);
    p.macroCrush = load (PID::macroCrush);
    p.filterCutoff = load (PID::filterCutoff);
    p.ampAttack = load (PID::ampAttack);
    p.ampDecay = load (PID::ampDecay);
    p.ampSustain = load (PID::ampSustain);
    p.ampRelease = load (PID::ampRelease);
    return p;
}

void DisklordzRomplerProcessor::refreshParameters() noexcept
{
    liveParams_ = readParamsFromApvts();
}

void DisklordzRomplerProcessor::pushParamsToEngine() noexcept
{
    engine_.setParams (liveParams_);
}

void DisklordzRomplerProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
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
        else if (msg.isPitchWheel())
        {
            const float norm = static_cast<float> (msg.getPitchWheelValue()) / 8192.0f - 1.0f;
            engine_.setPitchBendSemis (norm * 2.0f);
        }
        else if (msg.isController() && msg.getControllerNumber() == 64)
            engine_.setSustainPedal (msg.getControllerValue() >= 64);
    }

    engine_.render (left, right, numSamples);

    const float gain = juce::Decibels::decibelsToGain (liveParams_.outputGainDb, -100.0f);
    buffer.applyGain (gain);
}

juce::AudioProcessorEditor* DisklordzRomplerProcessor::createEditor()
{
    return new DisklordzRomplerEditor (*this);
}

void DisklordzRomplerProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    state.setProperty ("currentProgram", currentProgram_, nullptr);
    copyXmlToBinary (*state.createXml(), destData);
}

void DisklordzRomplerProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
    {
        if (xml->hasTagName (apvts_.state.getType()))
        {
            auto vt = juce::ValueTree::fromXml (*xml);
            currentProgram_ = static_cast<int> (vt.getProperty ("currentProgram", 0));
            apvts_.replaceState (vt);
            refreshParameters();
            pushParamsToEngine();
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DisklordzRomplerProcessor();
}
