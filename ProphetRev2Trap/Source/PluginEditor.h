#pragma once

#include "PluginProcessor.h"

class ProphetRev2TrapAudioProcessorEditor : public juce::AudioProcessorEditor,
                                            private juce::Timer
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

    void timerCallback() override;
    void addKnob (const char* paramId, const juce::String& label);
    void refreshPresetListForCategory (const juce::String& category, int selectLocalIndex);
    void rebuildFilteredPresetList (int selectLocalIndex);
    void syncUiToCurrentProgram();
    void updateCategoryHint();
    void updatePresetBrowserSummary();
    void updateActivePresetHeader();
    void updateFavoriteStar();
    void updateVoiceModeLabel();
    void onPresetSelected();
    void stepPreset (int delta);
    void toggleFavoriteForCurrent();
    void promptSaveUserPreset();
    juce::Colour categoryAccentColour (const juce::String& category) const;
    void layoutKnobGrid (juce::Rectangle<int> area, std::size_t startIndex, int count, int columns);

    ProphetRev2TrapAudioProcessor& processor_;
    juce::ComboBox categoryBox_;
    juce::ComboBox presetBox_;
    juce::TextEditor presetFilterEditor_;
    juce::Label presetFilterLabel_ { {}, "Search" };
    juce::String activeFactoryCategory_;
    juce::StringArray filteredPresetGlobals_;
    juce::TextEditor userPresetNameEditor_;
    juce::TextButton saveUserButton_ { "Save to User" };
    juce::TextButton prevPresetButton_ { "<" };
    juce::TextButton nextPresetButton_ { ">" };
    juce::ToggleButton favoriteStarButton_ { "Star" };
    juce::Label categoryLabel_ { {}, "Category" };
    juce::Label presetLabel_ { {}, "Preset" };
    juce::ToggleButton featuredOnlyToggle_ { "Featured" };
    juce::ToggleButton favoritesOnlyToggle_ { "Favorites" };
    juce::Label presetCountLabel_;
    juce::Label activePresetNameLabel_;
    juce::Label modifiedBadgeLabel_ { {}, "Modified" };
    juce::Label categoryHintLabel_;
    juce::Label voiceModeLabel_;
    juce::Label oscSectionLabel_ { {}, "Oscillators & filter" };
    juce::Label ampEnvLabel_ { {}, "Amp envelope" };
    juce::Label filtEnvLabel_ { {}, "Filter envelope" };
    juce::Label perfSectionLabel_ { {}, "Performance & circuit" };

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
