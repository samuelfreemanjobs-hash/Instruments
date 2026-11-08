#pragma once

#include "Engine/AutoRecordEngine.h"
#include "Engine/AutoSampleSessionPlan.h"
#include "Engine/MegaBankTemplate.h"
#include "Engine/MultisampleProgram.h"
#include "Engine/SamplerEngine.h"
#include "Export/MultisampleExporter.h"
#include "AutoSamplerConstants.h"
#include "Parameters/ParameterIds.h"

#include <JuceHeader.h>

#include <atomic>
#include <vector>

class AutoSamplerAudioProcessor final : public juce::AudioProcessor
{
public:
    AutoSamplerAudioProcessor();
    ~AutoSamplerAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getApvts() noexcept { return apvts_; }

    autosampler::MultisampleProgram& getProgram() noexcept { return presetBank_[static_cast<std::size_t> (currentProgram_)]; }
    const autosampler::MultisampleProgram& getProgram() const noexcept { return presetBank_[static_cast<std::size_t> (currentProgram_)]; }

    const std::vector<autosampler::MultisampleProgram>& getPresetBank() const noexcept { return presetBank_; }

    juce::File getExportRootDirectory() const { return exportRootDirectory_; }
    void setExportRootDirectory (const juce::File& dir) { exportRootDirectory_ = dir; }

    autosampler::ExportResult exportCurrentPreset() const;
    autosampler::ExportResult exportMpcKit() const;
    autosampler::ExportResult exportEntireBank() const;

    void queueAsyncExport (bool mpcKit);
    bool isExportInProgress() const noexcept { return exportInProgress_.load(); }
    juce::String lastExportMessage() const;

    void reprocessCurrentPreset() noexcept;
    bool writeMegaBankTemplateJson (const juce::File& jsonFile, juce::String& error) const;

    autosampler::AutoSampleState autoSampleState() const noexcept { return autoRecord_.state(); }
    float autoSampleProgress() const noexcept { return autoRecord_.progress01(); }
    autosampler::AutoSampleSessionPlan getSessionPlan() const noexcept;
    float inputPeakLevel() const noexcept { return inputPeak_.load(); }

    juce::MidiKeyboardState& getKeyboardState() noexcept { return keyboardState_; }

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    void refreshParametersFromApvts() noexcept;
    void handleTriggers() noexcept;
    autosampler::AutoRecordEngine::Config buildAutoConfig() const noexcept;
    void runExportJob (bool mpcKit);

    juce::AudioProcessorValueTreeState apvts_;
    std::vector<autosampler::MultisampleProgram> presetBank_;
    int currentProgram_ = 0;

    autosampler::AutoRecordEngine autoRecord_;
    autosampler::SamplerEngine sampler_;

    juce::File exportRootDirectory_;
    bool lastStartTrigger_ = false;
    bool lastExportTrigger_ = false;
    bool lastMpcKitTrigger_ = false;
    bool lastReprocessTrigger_ = false;

    std::atomic<bool> exportInProgress_ { false };
    std::atomic<float> inputPeak_ { 0.0f };
    mutable juce::CriticalSection exportMessageLock_;
    juce::String exportMessage_ { "Ready" };
    juce::MidiKeyboardState keyboardState_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoSamplerAudioProcessor)
};
