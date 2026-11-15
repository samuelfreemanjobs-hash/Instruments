#pragma once
#include "PluginProcessor.h"

class DrumDesignLabAudioProcessorEditor : public juce::AudioProcessorEditor,
                                          private juce::Timer
{
public:
    explicit DrumDesignLabAudioProcessorEditor(DrumDesignLabAudioProcessor&);
    ~DrumDesignLabAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void buildPadButtons();

    DrumDesignLabAudioProcessor& proc_;
    juce::ComboBox styleBox_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> styleAttach_;
    juce::Slider gainSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttach_;
    juce::Label title_, styleLabel_, gainLabel_, peakLabel_;
    std::array<juce::TextButton, 16> pads_;
    float peakHold_ = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumDesignLabAudioProcessorEditor)
};
