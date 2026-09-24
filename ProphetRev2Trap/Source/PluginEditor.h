#pragma once

#include "PluginProcessor.h"

class ProphetRev2TrapAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit ProphetRev2TrapAudioProcessorEditor (ProphetRev2TrapAudioProcessor&);
    ~ProphetRev2TrapAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    void addEnvelopeKnob (const char* paramId, juce::String label, juce::Rectangle<int> bounds);
    void refreshPresetListForCategory (const juce::String& category, int selectLocalIndex);
    void syncUiToCurrentProgram();

    ProphetRev2TrapAudioProcessor& processor_;
    juce::ComboBox categoryBox_;
    juce::ComboBox presetBox_;
    juce::Label categoryLabel_ { {}, "Category" };
    juce::Label presetLabel_ { {}, "Preset" };
    juce::Label categoryHintLabel_;
    juce::Label ampEnvLabel_ { {}, "Amp envelope" };
    juce::Label filtEnvLabel_ { {}, "Filter envelope" };

    void updateCategoryHint();

    juce::Slider outputSlider_;
    std::unique_ptr<SliderAttachment> outputAttachment_;

    std::vector<std::unique_ptr<juce::Slider>> envSliders_;
    std::vector<std::unique_ptr<SliderAttachment>> envAttachments_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProphetRev2TrapAudioProcessorEditor)
};
