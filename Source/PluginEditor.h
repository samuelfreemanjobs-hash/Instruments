#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "PluginProcessor.h"

#include <array>
#include <memory>

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
    std::array<juce::ToggleButton, 4> toneMuteButtons_;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, 4> toneMuteAttachments_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JDUpgradedAudioProcessorEditor)
};
