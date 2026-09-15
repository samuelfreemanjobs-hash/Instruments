#pragma once

#include <JuceHeader.h>

namespace vmpc::audio::internal
{
/** Shared stereo in/out layout for built-in mix processors. */
class InternalMixProcessor : public juce::AudioProcessor
{
public:
    explicit InternalMixProcessor(const juce::String& pluginName);

    const juce::String getName() const override { return name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

protected:
    static juce::AudioProcessor::BusesProperties stereoLayout();

    juce::String name;
};
} // namespace vmpc::audio::internal
