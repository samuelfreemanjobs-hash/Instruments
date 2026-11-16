#pragma once
#include "PluginProcessor.h"

/** Kit-factory UI: formula → audition pads → export pack. No workstation mode tabs. */
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
    void buildPads();
    void refreshPadHighlights();
    void exportCurrentKit();
    void exportAllKits();

    DrumDesignLabAudioProcessor& proc_;

    juce::Label title_, subtitle_, formulaLabel_, engineLabel_, statusLabel_, factoryTitle_, qaLabel_;
    juce::ComboBox formulaBox_, engineBox_;
    juce::Slider gainSlider_, variationsSlider_;
    juce::Label gainLabel_, variationsLabel_;
    juce::TextButton exportCurrentBtn_ { "Generate & Export Kit" };
    juce::TextButton exportAllBtn_ { "Export All 32 Formulas" };
    juce::ToggleButton useEngineToggle_ { "Use producer engine (optional)" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> formulaAttach_, engineAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttach_, variationsAttach_;

    std::array<juce::TextButton, 16> pads_;
    int lastPad_ = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumDesignLabAudioProcessorEditor)
};
