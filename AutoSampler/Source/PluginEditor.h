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

    AutoSamplerAudioProcessor& processor_;

    juce::Label titleLabel_ { {}, "Auto Sampler" };
    juce::Label statusLabel_ { {}, "Idle" };
    juce::ComboBox programBox_;
    juce::TextButton startButton_ { "Start Auto Sample" };
    juce::TextButton exportPresetButton_ { "Export Preset" };
    juce::TextButton exportBankButton_ { "Export Bank" };
    juce::TextButton chooseFolderButton_ { "Export Folder..." };

    juce::Slider lowNoteSlider_;
    juce::Slider highNoteSlider_;
    juce::Slider recordSecSlider_;
    juce::Slider outputGainSlider_;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<SliderAttachment> lowNoteAttachment_;
    std::unique_ptr<SliderAttachment> highNoteAttachment_;
    std::unique_ptr<SliderAttachment> recordSecAttachment_;
    std::unique_ptr<SliderAttachment> outputGainAttachment_;
    std::unique_ptr<ButtonAttachment> startAttachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoSamplerAudioProcessorEditor)
};
