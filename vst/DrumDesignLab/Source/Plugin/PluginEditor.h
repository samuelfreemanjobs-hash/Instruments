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
    void buildEditControls();
    void layoutKnob(juce::Slider& s, juce::Label& l, juce::Rectangle<int> area);
    void refreshPadHighlights();
    void pullKnobsIfNeeded();

    DrumDesignLabAudioProcessor& proc_;

    juce::ComboBox styleBox_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> styleAttach_;
    juce::Slider gainSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttach_;
    juce::Label title_, styleLabel_, gainLabel_, statusLabel_, editTitle_;

    std::array<juce::TextButton, 16> pads_;

    struct Knob
    {
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attach;
    };
    std::array<Knob, 15> knobs_;

    int lastStyle_ = -1;
    int lastPad_ = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumDesignLabAudioProcessorEditor)
};
