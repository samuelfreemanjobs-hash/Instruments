#pragma once

#include "PluginProcessor.h"

#include <JuceHeader.h>

class RenaissanceHostAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                                  private juce::Timer
{
public:
    explicit RenaissanceHostAudioProcessorEditor (RenaissanceHostAudioProcessor&);
    ~RenaissanceHostAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    RenaissanceHostAudioProcessor& processor_;
    juce::Label titleLabel_;
    juce::Label padLabel_;
    juce::Label qlinkLabel_;
    juce::Label lcdLabel_;
    std::array<juce::TextButton, renaissance::kNumPads> padButtons_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RenaissanceHostAudioProcessorEditor)
};
