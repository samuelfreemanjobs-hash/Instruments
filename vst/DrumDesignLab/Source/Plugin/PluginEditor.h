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
    void buildModeChrome();
    void layoutKnob(juce::Slider& s, juce::Label& l, juce::Rectangle<int> area);
    void refreshPadHighlights();
    void pullKnobsIfNeeded();
    void updateModeVisibility();
    void exportCurrentKit();
    void exportAllKits();

    DrumDesignLabAudioProcessor& proc_;

    juce::Label title_, statusLabel_, editTitle_, dnaTitle_, grooveTitle_, factoryTitle_;
    juce::ComboBox modeBox_, styleBox_, formulaBox_, destructionBox_, grooveBox_, dnaMatrixBox_;
    juce::Slider gainSlider_, masterDbSlider_, dnaX_, dnaY_, dnaInfl_, mutateStrength_, bpmSlider_, swingSlider_;
    juce::ToggleButton seqToggle_ { "Play Groove" }, bypassToggle_ { "Bypass" };
    juce::TextButton mutateBtn_ { "Mutate Pad" }, exportCurrentBtn_ { "Export Current Kit" },
        exportAllBtn_ { "Export All 32 DNA Kits" }, applyDnaBtn_ { "Apply DNA Morph" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttach_, styleAttach_,
        formulaAttach_, destructionAttach_, grooveAttach_, dnaMatrixAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttach_, masterAttach_,
        dnaXAttach_, dnaYAttach_, dnaInflAttach_, mutateAttach_, bpmAttach_, swingAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> seqAttach_, bypassAttach_;

    juce::Label styleLabel_, formulaLabel_, gainLabel_, masterLabel_;

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
    int lastMode_ = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumDesignLabAudioProcessorEditor)
};
