#pragma once

#include <JuceHeader.h>

#include "DrumRender.h"
#include "TrapForgePreset.h"

class TrapForgeAudioProcessor : public juce::AudioProcessor
{
public:
    TrapForgeAudioProcessor();
    ~TrapForgeAudioProcessor() override;

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

    const trapforge::TrapForgePreset& getActivePreset() const { return activePreset_; }

private:
    struct Voice
    {
        std::vector<float> mono;
        std::size_t position = 0;
    };

    void triggerVoice (float velocity);
    void refreshActivePreset();

    juce::Array<trapforge::TrapForgePreset> factoryPresets_;
    trapforge::TrapForgePreset activePreset_;
    int currentProgram_ = 0;
    double sampleRate_ = 44100.0;
    juce::CriticalSection voiceLock_;
    juce::Array<Voice> voices_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrapForgeAudioProcessor)
};
