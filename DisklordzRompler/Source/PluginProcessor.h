#pragma once

#include "Assets/RawRomBank.h"
#include "Engine/RomplerEngine.h"
#include "Engine/RomplerParams.h"
#include "Parameters/ParameterIds.h"
#include "Presets/FactoryPresets.h"

#include <JuceHeader.h>

class DisklordzRomplerProcessor final : public juce::AudioProcessor
{
public:
    DisklordzRomplerProcessor();
    ~DisklordzRomplerProcessor() override;

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

    void applyFactoryPreset (int index);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    void refreshParameters() noexcept;
    void pushParamsToEngine() noexcept;
    disklordz::rompler::RomplerParams readParamsFromApvts() const noexcept;

    juce::AudioProcessorValueTreeState apvts_;
    disklordz::rompler::assets::RawRomBank romBank_;
    disklordz::rompler::engine::RomplerEngine engine_;
    disklordz::rompler::RomplerParams liveParams_{};
    int currentProgram_ = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DisklordzRomplerProcessor)
};
