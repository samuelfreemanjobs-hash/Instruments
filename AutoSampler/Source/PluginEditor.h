#pragma once

#include "PluginProcessor.h"

#include <JuceHeader.h>

class AutoSamplerAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                              private juce::Timer
{
public:
    explicit AutoSamplerAudioProcessorEditor (AutoSamplerAudioProcessor&);
    ~AutoSamplerAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void styleSlider (juce::Slider& s);

    AutoSamplerAudioProcessor& processor_;

    juce::Label titleLabel_ { {}, "Auto Sampler" };
    juce::Label statusLabel_ { {}, "Idle" };
    juce::Label planLabel_ { {}, "Plan" };
    juce::Label inputMeterLabel_ { {}, "In: -inf dB" };
    juce::ComboBox programBox_;
    juce::ComboBox noteMapBox_;
    juce::ComboBox blackKeyBox_;
    juce::TextButton startButton_ { "Start Auto Sample" };
    juce::TextButton exportPresetButton_ { "Export Preset" };
    juce::TextButton exportMpcKitButton_ { "Export MPC Kit" };
    juce::TextButton exportBankButton_ { "Export Bank" };
    juce::TextButton chooseFolderButton_ { "Export Folder..." };
    juce::TextButton reprocessButton_ { "Reprocess" };
    juce::TextButton megaBankJsonButton_ { "Mega Bank JSON..." };
    juce::ProgressBar progressBar_;
    double progressValue_ = 0.0;
    juce::MidiKeyboardComponent keyboard_ { processor_.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard };

    juce::Slider lowNoteSlider_;
    juce::Slider highNoteSlider_;
    juce::Slider noteStepSlider_;
    juce::Slider gateMsSlider_;
    juce::Slider tailMsSlider_;
    juce::Slider prerollMsSlider_;
    juce::Slider pauseMsSlider_;
    juce::Slider velocity1Slider_;
    juce::Slider velocity2Slider_;
    juce::Slider velocity3Slider_;
    juce::Slider velocity4Slider_;
    juce::Slider midiChannelSlider_;
    juce::Slider outputGainSlider_;
    juce::ToggleButton trimButton_ { "Trim" };
    juce::ToggleButton normalizeButton_ { "Normalize" };
    juce::ToggleButton loopButton_ { "Sustain Loop" };
    juce::ToggleButton untilSilenceButton_ { "Until Silence" };

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::unique_ptr<ComboAttachment> noteMapAttachment_;
    std::unique_ptr<ComboAttachment> blackKeyAttachment_;
    std::unique_ptr<SliderAttachment> lowNoteAttachment_;
    std::unique_ptr<SliderAttachment> highNoteAttachment_;
    std::unique_ptr<SliderAttachment> noteStepAttachment_;
    std::unique_ptr<SliderAttachment> gateMsAttachment_;
    std::unique_ptr<SliderAttachment> tailMsAttachment_;
    std::unique_ptr<SliderAttachment> prerollMsAttachment_;
    std::unique_ptr<SliderAttachment> pauseMsAttachment_;
    std::unique_ptr<SliderAttachment> velocity1Attachment_;
    std::unique_ptr<SliderAttachment> velocity2Attachment_;
    std::unique_ptr<SliderAttachment> velocity3Attachment_;
    std::unique_ptr<SliderAttachment> velocity4Attachment_;
    std::unique_ptr<SliderAttachment> midiChannelAttachment_;
    std::unique_ptr<SliderAttachment> outputGainAttachment_;
    std::unique_ptr<ButtonAttachment> startAttachment_;
    std::unique_ptr<ButtonAttachment> trimAttachment_;
    std::unique_ptr<ButtonAttachment> normalizeAttachment_;
    std::unique_ptr<ButtonAttachment> loopAttachment_;
    std::unique_ptr<ButtonAttachment> untilSilenceAttachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoSamplerAudioProcessorEditor)
};
