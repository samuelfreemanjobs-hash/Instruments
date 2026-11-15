#pragma once
#include <JuceHeader.h>
#include <memory>
#include "../DSP/MultiGenreDrumEngine.h"

class DrumDesignLabAudioProcessor : public juce::AudioProcessor
{
public:
    DrumDesignLabAudioProcessor();
    ~DrumDesignLabAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "DiskLordz Drum Design Lab"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return static_cast<int>(disklordz::KitStyle::Count); }
    int getCurrentProgram() override { return static_cast<int>(currentStyle_); }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void setKitStyle(disklordz::KitStyle style);
    disklordz::KitStyle getKitStyle() const noexcept { return currentStyle_; }

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Heap: MultiGenre embeds large RoomReverb + 32 voices
    std::unique_ptr<disklordz::MultiGenreDrumEngine> engine_;
    disklordz::KitStyle currentStyle_ = disklordz::KitStyle::HitmakerEarthquake;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumDesignLabAudioProcessor)
};
