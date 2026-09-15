#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "PluginProcessor.h"

class JDUpgradedAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit JDUpgradedAudioProcessorEditor (JDUpgradedAudioProcessor&);
    ~JDUpgradedAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JDUpgradedAudioProcessor& processor_;
    juce::Slider masterGainSlider_;
    juce::Label titleLabel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JDUpgradedAudioProcessorEditor)
};
