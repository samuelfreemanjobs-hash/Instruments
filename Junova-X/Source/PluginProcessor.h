#pragma once

#include "DSP/Arpeggiator.h"
#include "DSP/ScopeFifo.h"
#include "DSP/SynthEngine.h"
#include "Parameters/ParameterIds.h"

#include <JuceHeader.h>

#include <atomic>

class JunovaXAudioProcessor final : public juce::AudioProcessor
{
public:
    JunovaXAudioProcessor();
    ~JunovaXAudioProcessor() override;

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

    void panicAllNotes();
    void applyFactoryPreset (int index);
    /** Competitive golden / A/B patches — see GoldenScenarios.h */
    bool applyGoldenScenario (const char* scenarioId) noexcept;
    void pushParamsToEngine() noexcept;

    void copyScopeSamples (float* dest, int numSamples) const noexcept
    {
        scopeFifo_.copyRecent (dest, numSamples);
    }

    float getDisplayBpm() const noexcept { return displayBpm_.load (std::memory_order_relaxed); }
    double getEffectiveSampleRate() const noexcept { return lastSampleRate_; }

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    junovax::dsp::RuntimeParams readParamsFromApvts() const noexcept;

    juce::AudioProcessorValueTreeState apvts_;
    junovax::dsp::Arpeggiator arpeggiator_;
    junovax::dsp::SynthEngine engine_;
    junovax::dsp::ScopeFifo scopeFifo_;
    int currentProgram_ = 0;
    std::atomic<float> displayBpm_ { 120.0f };
    double lastSampleRate_ = 48000.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JunovaXAudioProcessor)
};
