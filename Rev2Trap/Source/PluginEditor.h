#pragma once

#include "PluginProcessor.h"

class Rev2TrapAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit Rev2TrapAudioProcessorEditor (Rev2TrapAudioProcessor&);
    ~Rev2TrapAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    Rev2TrapAudioProcessor& processor_;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Rev2TrapAudioProcessorEditor)
};
