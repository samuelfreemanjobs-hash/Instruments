#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "PluginProcessor.h"
#include "UI/WavePreviewComponent.h"

#include <array>
#include <memory>

class JDUpgradedAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                             private juce::Timer
{
public:
    explicit JDUpgradedAudioProcessorEditor (JDUpgradedAudioProcessor&);
    ~JDUpgradedAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void changeProgramByDelta (int delta);
    void styleRotary (juce::Slider& slider, const juce::String& name);
    void styleEnvSlider (juce::Slider& slider, const juce::String& name);
    void rebindEnvelopeAttachments();
    void updateFilterResonanceSliderVisibility();
    void snapToneToPaletteCategory (int toneIndex0Based, int comboItemId);
    juce::String describeWaveForTone (int toneIndex0Based) const;
    void updateRomBrowserPreview();
    void nudgeRomBrowseWave (int delta);
    void exportPreset();
    void importPreset();
    void applyPaletteCategoryToTone (int toneIndex0Based, int comboItemId);

    JDUpgradedAudioProcessor& processor_;
    bool multiPaletteLink_ = false;
    juce::Slider masterGainSlider_;
    juce::Slider filterResonanceSlider_;
    juce::ToggleButton filterLinkButton_;
    juce::Slider groupADriveSlider_;
    juce::Slider groupBMixSlider_;
    juce::Slider groupBChorusSlider_;
    juce::ToggleButton groupAEnableButton_;
    juce::ToggleButton groupBEnableButton_;
    juce::ComboBox couplingCombo_;
    juce::Label titleLabel_;
    juce::Label romSourceLabel_;
    juce::Label programLabel_;
    juce::TextButton programPrev_ { "<" };
    juce::TextButton programNext_ { ">" };
    juce::TextButton exportPresetButton_ { "Export" };
    juce::TextButton importPresetButton_ { "Import" };
    juce::Label romBrowseLabel_;
    juce::ComboBox romBrowseToneCombo_;
    juce::Slider romBrowseWaveSlider_;
    juce::TextButton romWavePrev_ { "<" };
    juce::TextButton romWaveNext_ { ">" };
    WavePreviewComponent romWavePreview_;
    std::array<juce::Slider, 4> toneLevelSliders_;
    std::array<juce::Slider, 4> toneWaveSliders_;
    std::array<juce::Slider, 4> toneMultisampleSliders_;
    std::array<juce::Slider, 4> toneCutoffSliders_;
    std::array<juce::Slider, 4> toneResonanceSliders_;
    std::array<juce::ToggleButton, 4> toneMuteButtons_;
    std::array<juce::ComboBox, 4> tonePaletteCombos_;
    std::array<juce::Label, 4> tonePaletteLabels_;
    juce::Label ampEnvLabel_;
    juce::Label filterEnvLabel_;
    juce::ToggleButton envelopeLinkButton_;
    juce::ToggleButton multiPaletteLinkButton_;
    juce::ComboBox envelopeToneCombo_;
    juce::Label envelopeToneLabel_;
    juce::Slider ampAttackSlider_;
    juce::Slider ampDecaySlider_;
    juce::Slider ampSustainSlider_;
    juce::Slider ampReleaseSlider_;
    juce::Slider filterAttackSlider_;
    juce::Slider filterDecaySlider_;
    juce::Slider filterSustainSlider_;
    juce::Slider filterReleaseSlider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> filterLinkAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> groupAAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> groupBAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> groupBChorusAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> groupAEnableAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> groupBEnableAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> couplingAttachment_;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 4> toneLevelAttachments_;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 4> toneWaveAttachments_;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 4> toneMsAttachments_;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 4> toneCutoffAttachments_;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 4> toneResonanceAttachments_;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, 4> toneMuteAttachments_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampAttackAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampDecayAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampSustainAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampReleaseAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterAttackAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterDecayAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterSustainAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterReleaseAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> envelopeLinkAttachment_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JDUpgradedAudioProcessorEditor)
};
