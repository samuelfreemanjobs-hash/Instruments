#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "DSP/FMEngine.h"
#include "Parameters/ParameterLayout.h"

class CharacterFMAudioProcessor final : public juce::AudioProcessor
{
public:
    CharacterFMAudioProcessor();
    ~CharacterFMAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return "Init"; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() noexcept { return apvts_; }

    bool exportPresetJsonToFile (const juce::File& file);
    bool importPresetJsonFromFile (const juce::File& file);

private:
    void handleMidi (const juce::MidiBuffer& midi) noexcept;
    void syncEngineParams() noexcept;

    juce::AudioProcessorValueTreeState apvts_;
    characterfm::dsp::FMEngine engine_;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> masterSmoothed_;

    std::atomic<float>* masterGainPtr_ = nullptr;
    std::atomic<float>* algorithmPtr_ = nullptr;
    std::array<std::atomic<float>*, 6> ratioPtrs_{};
    std::array<std::atomic<float>*, 6> levelPtrs_{};
};
