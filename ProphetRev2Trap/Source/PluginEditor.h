#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class ProphetRev2TrapAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit ProphetRev2TrapAudioProcessorEditor(ProphetRev2TrapAudioProcessor&);
    ~ProphetRev2TrapAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ProphetRev2TrapAudioProcessor& processor_;
    juce::ComboBox programBox_;
    juce::Slider cutoffSlider_;
    juce::Slider resSlider_;
    juce::Slider slopSlider_;
    juce::Slider glideSlider_;
    juce::ComboBox satBox_;

    using Attach = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attach> cutoffAttach_;
    std::unique_ptr<Attach> resAttach_;
    std::unique_ptr<Attach> slopAttach_;
    std::unique_ptr<Attach> glideAttach_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> satAttach_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProphetRev2TrapAudioProcessorEditor)
};
