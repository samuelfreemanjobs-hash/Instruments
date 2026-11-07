#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AutoSamplerConstants.h"

#include <algorithm>

namespace
{
using PID = autosampler::ParameterIDs;
using autosampler::kBankSlots;

void addIntParam (juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                  const char* id,
                  const juce::String& name,
                  int minV,
                  int maxV,
                  int defaultV)
{
    layout.add (std::make_unique<juce::AudioParameterInt> (juce::ParameterID { id, 1 }, name, minV, maxV, defaultV));
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

juce::AudioProcessorValueTreeState::ParameterLayout AutoSamplerAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    addFloatParam (layout, PID::outputGain, "Output Gain (dB)", { -24.0f, 12.0f, 0.01f }, 0.0f);
    addIntParam (layout, PID::lowNote, "Low Note", 0, 127, 36);
    addIntParam (layout, PID::highNote, "High Note", 0, 127, 72);
    addIntParam (layout, PID::noteStep, "Note Step", 1, 12, 1);
    addFloatParam (layout, PID::recordSeconds, "Record Seconds", { 0.25f, 30.0f, 0.01f }, 2.0f);
    addFloatParam (layout, PID::gapMs, "Gap (ms)", { 0.0f, 2000.0f, 1.0f }, 150.0f);
    addIntParam (layout, PID::recordVelocity, "Record Velocity", 1, 127, 100);
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::midiThru, 1 }, "MIDI Thru", true));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::startAutoSample, 1 }, "Start Auto Sample", false));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::exportPreset, 1 }, "Export Preset", false));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::trimSilence, 1 }, "Trim Silence", true));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::normalizeCapture, 1 }, "Normalize Capture", true));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::sustainLoop, 1 }, "Sustain Loop", true));
    addFloatParam (layout, PID::trimThresholdDb, "Trim Threshold (dB)", { -72.0f, -18.0f, 0.1f }, -48.0f);
    addFloatParam (layout, PID::trimPadMs, "Trim Pad (ms)", { 0.0f, 50.0f, 0.1f }, 5.0f);
    addFloatParam (layout, PID::normalizePeakDb, "Normalize Peak (dB)", { -6.0f, 0.0f, 0.01f }, -0.3f);

    return layout;
}

AutoSamplerAudioProcessor::AutoSamplerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
#endif
      ,
      apvts_ (*this, nullptr, "AutoSampler", createParameterLayout())
{
    presetBank_.resize (static_cast<std::size_t> (kBankSlots));
    for (int i = 0; i < kBankSlots; ++i)
        presetBank_[static_cast<std::size_t> (i)].name = "Preset " + juce::String (i + 1);

    exportRootDirectory_ = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)
                               .getChildFile ("AutoSamplerExports");
}

AutoSamplerAudioProcessor::~AutoSamplerAudioProcessor() = default;

const juce::String AutoSamplerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AutoSamplerAudioProcessor::acceptsMidi() const { return true; }
bool AutoSamplerAudioProcessor::producesMidi() const { return true; }
bool AutoSamplerAudioProcessor::isMidiEffect() const { return false; }
double AutoSamplerAudioProcessor::getTailLengthSeconds() const { return 2.0; }
bool AutoSamplerAudioProcessor::hasEditor() const { return true; }

int AutoSamplerAudioProcessor::getNumPrograms() { return kBankSlots; }
int AutoSamplerAudioProcessor::getCurrentProgram() { return currentProgram_; }

void AutoSamplerAudioProcessor::setCurrentProgram (int index)
{
    currentProgram_ = juce::jlimit (0, kBankSlots - 1, index);
    sampler_.setProgram (&getProgram());
}

const juce::String AutoSamplerAudioProcessor::getProgramName (int index)
{
    if (index < 0 || index >= kBankSlots)
        return {};
    return presetBank_[static_cast<std::size_t> (index)].name;
}

void AutoSamplerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    if (index >= 0 && index < kBankSlots)
        presetBank_[static_cast<std::size_t> (index)].name = newName;
}

void AutoSamplerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    autoRecord_.prepare (sampleRate, samplesPerBlock);
    sampler_.prepare (sampleRate, samplesPerBlock);
    sampler_.setProgram (&getProgram());
    refreshParametersFromApvts();
}

void AutoSamplerAudioProcessor::releaseResources() {}

bool AutoSamplerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled())
        return true;

    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono()
           || layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo();
}

autosampler::AutoRecordEngine::Config AutoSamplerAudioProcessor::buildAutoConfig() const noexcept
{
    autosampler::AutoRecordEngine::Config cfg;
    cfg.lowNote = static_cast<int> (*apvts_.getRawParameterValue (PID::lowNote));
    cfg.highNote = static_cast<int> (*apvts_.getRawParameterValue (PID::highNote));
    cfg.noteStep = static_cast<int> (*apvts_.getRawParameterValue (PID::noteStep));
    cfg.recordSeconds = static_cast<double> (*apvts_.getRawParameterValue (PID::recordSeconds));
    cfg.gapSeconds = static_cast<double> (*apvts_.getRawParameterValue (PID::gapMs)) / 1000.0;
    cfg.velocity = static_cast<int> (*apvts_.getRawParameterValue (PID::recordVelocity));
    cfg.midiThru = *apvts_.getRawParameterValue (PID::midiThru) >= 0.5f;
    cfg.postProcess.trimSilence = *apvts_.getRawParameterValue (PID::trimSilence) >= 0.5f;
    cfg.postProcess.normalize = *apvts_.getRawParameterValue (PID::normalizeCapture) >= 0.5f;
    cfg.postProcess.findSustainLoop = *apvts_.getRawParameterValue (PID::sustainLoop) >= 0.5f;
    cfg.postProcess.trimThresholdDb = *apvts_.getRawParameterValue (PID::trimThresholdDb);
    cfg.postProcess.trimPadMs = *apvts_.getRawParameterValue (PID::trimPadMs);
    cfg.postProcess.normalizePeakDb = *apvts_.getRawParameterValue (PID::normalizePeakDb);
    return cfg;
}

void AutoSamplerAudioProcessor::handleTriggers() noexcept
{
    const bool startTrig = *apvts_.getRawParameterValue (PID::startAutoSample) >= 0.5f;
    if (startTrig && ! lastStartTrigger_)
    {
        getProgram().clear();
        autoRecord_.start (buildAutoConfig());
    }
    lastStartTrigger_ = startTrig;

    const bool exportTrig = *apvts_.getRawParameterValue (PID::exportPreset) >= 0.5f;
    if (exportTrig && ! lastExportTrigger_)
        exportCurrentPreset();
    lastExportTrigger_ = exportTrig;
}

void AutoSamplerAudioProcessor::refreshParametersFromApvts() noexcept
{
    sampler_.setOutputGainDb (*apvts_.getRawParameterValue (PID::outputGain));
}

void AutoSamplerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    handleTriggers();

    juce::AudioBuffer<float> input (getTotalNumInputChannels(), buffer.getNumSamples());
    for (int ch = 0; ch < input.getNumChannels(); ++ch)
        input.copyFrom (ch, 0, buffer, ch, 0, buffer.getNumSamples());

    juce::MidiBuffer midiThru;
    if (autoRecord_.isActive() || autoRecord_.state() == autosampler::AutoSampleState::finished)
        autoRecord_.process (input, midiThru, getProgram());

    midi.addEvents (midiThru, 0, buffer.getNumSamples(), 0);

    sampler_.process (buffer, midi);
    refreshParametersFromApvts();
}

juce::AudioProcessorEditor* AutoSamplerAudioProcessor::createEditor()
{
    return new AutoSamplerAudioProcessorEditor (*this);
}

autosampler::ExportResult AutoSamplerAudioProcessor::exportCurrentPreset() const
{
    if (exportRootDirectory_.getFullPathName().isEmpty())
        return { false, "Export directory not set", {} };

    exportRootDirectory_.createDirectory();
    return autosampler::MultisampleExporter::exportPreset (getProgram(), exportRootDirectory_);
}

autosampler::ExportResult AutoSamplerAudioProcessor::exportEntireBank() const
{
    autosampler::BankExportSpec spec;
    spec.bankName = "PresetBank";
    spec.presets = presetBank_;
    spec.presets.erase (std::remove_if (spec.presets.begin(), spec.presets.end(),
                                        [] (const autosampler::MultisampleProgram& p) { return p.empty(); }),
                        spec.presets.end());

    exportRootDirectory_.createDirectory();
    return autosampler::MultisampleExporter::exportBank (spec, exportRootDirectory_);
}

void AutoSamplerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ValueTree root { "AutoSamplerState" };
    root.setProperty ("exportRoot", exportRootDirectory_.getFullPathName(), nullptr);
    root.setProperty ("currentProgram", currentProgram_, nullptr);

    juce::ValueTree bank { "Bank" };
    for (int i = 0; i < kBankSlots; ++i)
        bank.appendChild (presetBank_[static_cast<std::size_t> (i)].toValueTree(), nullptr);

    root.appendChild (bank, nullptr);
    root.appendChild (apvts_.copyState(), nullptr);

    if (auto xml = root.createXml())
        copyXmlToBinary (*xml, destData);
}

void AutoSamplerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
    {
        const juce::ValueTree root = juce::ValueTree::fromXml (*xml);
        exportRootDirectory_ = juce::File (root.getProperty ("exportRoot").toString());
        currentProgram_ = static_cast<int> (root.getProperty ("currentProgram", 0));

        const auto bank = root.getChildWithName ("Bank");
        for (int i = 0; i < bank.getNumChildren() && i < kBankSlots; ++i)
            presetBank_[static_cast<std::size_t> (i)].loadFromValueTree (bank.getChild (i));

        const auto params = root.getChildWithName (apvts_.state.getType());
        if (params.isValid())
            apvts_.replaceState (params);

        setCurrentProgram (currentProgram_);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AutoSamplerAudioProcessor();
}
