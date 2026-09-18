#pragma once

#include "Engine/SynthEngine.h"
#include "Engine/SynthParams.h"
#include "Parameters/ParameterIds.h"
#include "Presets/FactoryPresets.h"

#include <JuceHeader.h>

class Wave909AudioProcessor final : public juce::AudioProcessor
{
public:
    Wave909AudioProcessor();
    ~Wave909AudioProcessor() override;

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
    const wave909::SynthParams& getLiveParams() const noexcept { return liveParams_; }

    void applyFactoryPreset (int index);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    void refreshParameters() noexcept;
    void pushParamsToEngine() noexcept;
    wave909::SynthParams readParamsFromApvts() const noexcept;
    void applyParameterSmoothing (int numSamples) noexcept;

    juce::AudioProcessorValueTreeState apvts_;
    wave909::engine::SynthEngine engine_;
    wave909::SynthParams liveParams_{};
    int currentProgram_ = 0;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedCutoff_;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedWtPosition_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wave909AudioProcessor)
};
