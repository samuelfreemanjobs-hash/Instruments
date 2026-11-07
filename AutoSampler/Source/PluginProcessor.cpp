#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "AutoSamplerConstants.h"
#include "Engine/NoteMapBuilder.h"

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
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::noteMapPreset, 1 },
                                                              "Note Map",
                                                              juce::StringArray { "Chromatic", "Minor 3rd", "Major 3rd", "Fifth", "Octave" },
                                                              0));
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::blackKeyPolicy, 1 },
                                                              "Black Keys",
                                                              juce::StringArray { "Include", "Skip", "White Only" },
                                                              0));
    addFloatParam (layout, PID::noteHoldMs, "Gate (ms)", { 50.0f, 10000.0f, 1.0f }, 1000.0f);
    addFloatParam (layout, PID::tailMs, "Release (ms)", { 50.0f, 30000.0f, 1.0f }, 2000.0f);
    addFloatParam (layout, PID::prerollMs, "Preroll (ms)", { 0.0f, 5000.0f, 1.0f }, 0.0f);
    addFloatParam (layout, PID::maxRecordSeconds, "Max Record Sec", { 1.0f, 120.0f, 0.1f }, 20.0f);
    addFloatParam (layout, PID::gapMs, "Pause (ms)", { 0.0f, 5000.0f, 1.0f }, 150.0f);
    addIntParam (layout, PID::recordVelocity, "Record Velocity", 0, 127, 127);
    addIntParam (layout, PID::velocity1, "Velocity 1", 0, 127, 127);
    addIntParam (layout, PID::velocity2, "Velocity 2", 0, 127, 0);
    addIntParam (layout, PID::velocity3, "Velocity 3", 0, 127, 0);
    addIntParam (layout, PID::velocity4, "Velocity 4", 0, 127, 0);
    addIntParam (layout, PID::midiChannel, "MIDI Channel", 1, 16, 1);
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::recordUntilSilence, 1 }, "Record Until Silence", false));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::midiThru, 1 }, "MIDI Thru", true));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::startAutoSample, 1 }, "Start Auto Sample", false));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::exportPreset, 1 }, "Export Preset", false));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::exportMpcKit, 1 }, "Export MPC Kit", false));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::reprocessPreset, 1 }, "Reprocess Capture", false));
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

AutoSamplerAudioProcessor::~AutoSamplerAudioProcessor()
{
    while (exportInProgress_.load())
        juce::Thread::sleep (10);
}

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
    cfg.noteHoldMs = static_cast<double> (*apvts_.getRawParameterValue (PID::noteHoldMs));
    cfg.tailMs = static_cast<double> (*apvts_.getRawParameterValue (PID::tailMs));
    cfg.prerollMs = static_cast<double> (*apvts_.getRawParameterValue (PID::prerollMs));
    cfg.pauseMs = static_cast<double> (*apvts_.getRawParameterValue (PID::gapMs));
    cfg.maxRecordSeconds = static_cast<double> (*apvts_.getRawParameterValue (PID::maxRecordSeconds));
    cfg.recordSeconds = (cfg.noteHoldMs + cfg.tailMs + cfg.prerollMs) / 1000.0;
    cfg.velocity = static_cast<int> (*apvts_.getRawParameterValue (PID::recordVelocity));
    cfg.velocitySlots[0] = static_cast<int> (*apvts_.getRawParameterValue (PID::velocity1));
    cfg.velocitySlots[1] = static_cast<int> (*apvts_.getRawParameterValue (PID::velocity2));
    cfg.velocitySlots[2] = static_cast<int> (*apvts_.getRawParameterValue (PID::velocity3));
    cfg.velocitySlots[3] = static_cast<int> (*apvts_.getRawParameterValue (PID::velocity4));
    cfg.midiChannel = static_cast<int> (*apvts_.getRawParameterValue (PID::midiChannel));
    cfg.midiThru = *apvts_.getRawParameterValue (PID::midiThru) >= 0.5f;
    cfg.recordUntilSilence = *apvts_.getRawParameterValue (PID::recordUntilSilence) >= 0.5f;
    cfg.maxRecordSeconds = static_cast<double> (*apvts_.getRawParameterValue (PID::maxRecordSeconds));
    cfg.noteMapPreset = static_cast<autosampler::NoteMapPreset> (
        static_cast<int> (*apvts_.getRawParameterValue (PID::noteMapPreset)));
    cfg.blackKeyPolicy = static_cast<autosampler::BlackKeyPolicy> (
        static_cast<int> (*apvts_.getRawParameterValue (PID::blackKeyPolicy)));
    cfg.postProcess.trimSilence = *apvts_.getRawParameterValue (PID::trimSilence) >= 0.5f;
    cfg.postProcess.normalize = *apvts_.getRawParameterValue (PID::normalizeCapture) >= 0.5f;
    cfg.postProcess.findSustainLoop = *apvts_.getRawParameterValue (PID::sustainLoop) >= 0.5f;
    cfg.postProcess.trimThresholdDb = *apvts_.getRawParameterValue (PID::trimThresholdDb);
    cfg.postProcess.trimPadMs = *apvts_.getRawParameterValue (PID::trimPadMs);
    cfg.postProcess.normalizePeakDb = *apvts_.getRawParameterValue (PID::normalizePeakDb);
    return cfg;
}

autosampler::AutoSampleSessionPlan AutoSamplerAudioProcessor::getSessionPlan() const noexcept
{
    return autosampler::AutoSampleSessionPlanBuilder::plan (buildAutoConfig());
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
        queueAsyncExport (false);
    lastExportTrigger_ = exportTrig;

    const bool mpcTrig = *apvts_.getRawParameterValue (PID::exportMpcKit) >= 0.5f;
    if (mpcTrig && ! lastMpcKitTrigger_)
        queueAsyncExport (true);
    lastMpcKitTrigger_ = mpcTrig;

    const bool reprocTrig = *apvts_.getRawParameterValue (PID::reprocessPreset) >= 0.5f;
    if (reprocTrig && ! lastReprocessTrigger_)
        reprocessCurrentPreset();
    lastReprocessTrigger_ = reprocTrig;
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
    float peak = 0.0f;
    for (int ch = 0; ch < input.getNumChannels(); ++ch)
    {
        input.copyFrom (ch, 0, buffer, ch, 0, buffer.getNumSamples());
        peak = juce::jmax (peak, input.getMagnitude (ch, 0, buffer.getNumSamples()));
    }
    inputPeak_.store (peak);

    juce::MidiBuffer midiThru;
    if (autoRecord_.isActive() || autoRecord_.state() == autosampler::AutoSampleState::finished)
        autoRecord_.process (input, midiThru, getProgram());

    midi.addEvents (midiThru, 0, buffer.getNumSamples(), 0);

    keyboardState_.processNextMidiBuffer (midi, 0, buffer.getNumSamples(), true);
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

autosampler::ExportResult AutoSamplerAudioProcessor::exportMpcKit() const
{
    if (exportRootDirectory_.getFullPathName().isEmpty())
        return { false, "Export directory not set", {} };

    exportRootDirectory_.createDirectory();
    return autosampler::MultisampleExporter::exportMpcKit (getProgram(), exportRootDirectory_, true);
}

void AutoSamplerAudioProcessor::queueAsyncExport (bool mpcKit)
{
    if (exportInProgress_.exchange (true))
        return;

    juce::Thread::launch ([this, mpcKit]
                          {
                              runExportJob (mpcKit);
                              exportInProgress_.store (false);
                          });
}

void AutoSamplerAudioProcessor::runExportJob (bool mpcKit)
{
    const auto result = mpcKit ? exportMpcKit() : exportCurrentPreset();
    const juce::ScopedLock lock (exportMessageLock_);
    exportMessage_ = result.ok ? ("Exported: " + result.presetJson.getFullPathName())
                               : ("Export failed: " + result.error);
}

juce::String AutoSamplerAudioProcessor::lastExportMessage() const
{
    const juce::ScopedLock lock (exportMessageLock_);
    return exportMessage_;
}

void AutoSamplerAudioProcessor::reprocessCurrentPreset() noexcept
{
    getProgram().reprocessAllZones (buildAutoConfig().postProcess);
}

bool AutoSamplerAudioProcessor::writeMegaBankTemplateJson (const juce::File& jsonFile, juce::String& error) const
{
    return autosampler::writeMegaBankJsonTemplate (jsonFile, error);
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
