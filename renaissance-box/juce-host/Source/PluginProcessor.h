#pragma once

#include "LcdBridge.h"
#include "PadVoiceEngine.h"
#include "RenaissanceSurfaceMap.h"

#include <JuceHeader.h>

class RenaissanceHostAudioProcessor final : public juce::AudioProcessor
{
public:
    RenaissanceHostAudioProcessor();
    ~RenaissanceHostAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Renaissance Host"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override {}
    void setStateInformation (const void*, int) override {}

    renaissance::RenaissanceSurfaceMap& surfaceMap() noexcept { return surfaceMap_; }
    renaissance::LcdBridge& lcdBridge() noexcept { return lcdBridge_; }

    juce::CriticalSection activityLock_;
    int lastPadIndex_ = -1;
    float lastVelocity_ = 0.0f;
    int lastQlinkPad_ = -1;
    float lastQlinkValue_ = 0.0f;

private:
    void handleMidiMessage (const juce::MidiMessage& msg);

    renaissance::RenaissanceSurfaceMap surfaceMap_;
    renaissance::PadVoiceEngine engine_;
    renaissance::LcdBridge lcdBridge_;
    std::unique_ptr<juce::MidiOutput> midiOut_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RenaissanceHostAudioProcessor)
};
