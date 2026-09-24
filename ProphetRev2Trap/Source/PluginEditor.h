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
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    void addKnob (const char* paramId, const juce::String& label);
    void refreshPresetListForCategory (const juce::String& category, int selectLocalIndex);
    void rebuildFilteredPresetList (int selectLocalIndex);
    void syncUiToCurrentProgram();
    void updateCategoryHint();
    void onPresetSelected();
    void promptSaveUserPreset();

    ProphetRev2TrapAudioProcessor& processor_;
    juce::ComboBox categoryBox_;
    juce::ComboBox presetBox_;
    juce::TextEditor presetFilterEditor_;
    juce::Label presetFilterLabel_ { {}, "Filter" };
    juce::String activeFactoryCategory_;
    juce::StringArray filteredPresetGlobals_;
    juce::TextEditor userPresetNameEditor_;
    juce::TextButton saveUserButton_ { "Save to User" };
    juce::Label categoryLabel_ { {}, "Category" };
    juce::Label presetLabel_ { {}, "Preset" };
    juce::Label categoryHintLabel_;
    juce::Label ampEnvLabel_ { {}, "Amp envelope" };
    juce::Label filtEnvLabel_ { {}, "Filter envelope" };
    juce::Label analogLabel_ { {}, "Analog circuit" };

    juce::Slider outputSlider_;
    std::unique_ptr<SliderAttachment> outputAttachment_;
    juce::ToggleButton monoButton_ { "Mono" };
    juce::ToggleButton legatoButton_ { "Legato" };
    std::unique_ptr<ButtonAttachment> monoAttachment_;
    std::unique_ptr<ButtonAttachment> legatoAttachment_;
    juce::ComboBox unisonBox_;
    std::unique_ptr<ComboAttachment> unisonAttachment_;

    std::vector<std::unique_ptr<juce::Slider>> knobs_;
    std::vector<std::unique_ptr<juce::Label>> knobLabels_;
    std::vector<std::unique_ptr<SliderAttachment>> knobAttachments_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProphetRev2TrapAudioProcessorEditor)
};
