#pragma once

#include "Engine/SamplerEngine.h"

#include <juce_audio_processors/juce_audio_processors.h>

class SP1200AudioProcessor : public juce::AudioProcessor
{
public:
    SP1200AudioProcessor();
    ~SP1200AudioProcessor() override;

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

    sp1200::SamplerEngine& engine() noexcept { return engine_; }
    [[nodiscard]] const sp1200::SamplerEngine& engine() const noexcept { return engine_; }

    void setFaderMode (int mode) { engine_.setFaderMode (mode); }
    void triggerPad (int padIndex, float velocity = 0.9f) { engine_.triggerPadFromUi (padIndex, velocity); }

private:
    sp1200::SamplerEngine engine_;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SP1200AudioProcessor)
};
