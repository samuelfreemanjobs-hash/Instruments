#pragma once

#include "DSP/Rev2Voice.h"
#include "Parameters/ParameterIds.h"
#include "Presets/FactoryPresets.h"

#include <juce_audio_processors/juce_audio_processors.h>

#include <array>
#include <atomic>

class Rev2TrapAudioProcessor final : public juce::AudioProcessor
{
public:
    Rev2TrapAudioProcessor();
    ~Rev2TrapAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override;
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 3.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override {}
    void setStateInformation (const void*, int) override {}

    juce::AudioProcessorValueTreeState& getAPVTS() noexcept { return apvts_; }

private:
    void applyPreset (int index);
    void refreshVoiceParams();

    juce::AudioProcessorValueTreeState apvts_;
    std::array<rev2trap::dsp::Rev2Voice, 16> voices_;
    rev2trap::presets::SynthParams currentParams_;
    std::atomic<int> currentProgram_ { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Rev2TrapAudioProcessor)
};
