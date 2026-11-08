#pragma once

#include "SynthProcessor.h"
#include "MoogStackLineage.h"
#include <JuceHeader.h>
#include <array>

class MeterScopePanel : public juce::Component
{
public:
    explicit MeterScopePanel(MoogVoyagerAudioProcessor& p) : processor(p) {}

    void setAccentColour(juce::Colour c) noexcept { accent = c; }

    void paint(juce::Graphics& g) override;

private:
    MoogVoyagerAudioProcessor& processor;
    juce::Colour accent { 0xfff4a62a };
};

class MoogVoyagerAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::AudioProcessorValueTreeState::Listener,
                                        private juce::Timer
{
public:
    explicit MoogVoyagerAudioProcessorEditor(MoogVoyagerAudioProcessor&);
    ~MoogVoyagerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void refreshPatchReadout();
    void refreshStackChrome();
    juce::Colour accentForCurrentStack() const;
    void selectStackMode(int index, bool applyTemplate);
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void timerCallback() override;

private:
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    static juce::Colour accentForLineage(moogstack::Lineage lineage);
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    static void styleKnob(juce::Slider& s, const juce::String& name);
    static void styleMixFader(juce::Slider& s, const juce::String& name);
    static void styleEnvSlider(juce::Slider& s, const juce::String& name);
    static void bindCombo(juce::ComboBox& box, const juce::StringArray& items);
    static void sectionLabel(juce::Label& label, const juce::String& text);
    void applyAccentToSlider(juce::Slider& s, juce::Colour accent, bool isFader) const;

    MoogVoyagerAudioProcessor& processorRef;

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::Label patchReadoutLabel;
    juce::Label stackBadgeLabel;
    juce::Label stackSwitchLabel;
    std::array<juce::TextButton, 4> stackModeSwitch;
    juce::Label presetLabel;
    juce::ComboBox presetBox;
    juce::TextButton presetPrevButton;
    juce::TextButton presetNextButton;
    juce::Label filterEnvRowLabel;
    juce::Label ampEnvRowLabel;
    juce::Label oversampleLabel;
    juce::ComboBox oversampleBox;
    MeterScopePanel meterScope;
    juce::ToggleButton noisePinkButton;
    juce::ToggleButton mixerOdButton;
    juce::ToggleButton filterOdButton;
    juce::Label modLfoLabel;
    juce::Label oscSectionLabel, filterSectionLabel, envSectionLabel, modSectionLabel;

    juce::Slider masterSlider;
    juce::Slider oscLevel[3], oscFine[3], noiseSlider;
    juce::ComboBox oscOctave[3], oscWave[3];
    juce::Label oscIndexLabel[3], oscOctLabel[3], oscWaveLabel[3];

    juce::Slider filterCutoff, filterRes, filterDrive, filterEnvAmt, filterKb, mixerDriveSlider, subOscSlider;
    juce::Slider fAttack, fDecay, fSustain, fRelease;
    juce::Slider aAttack, aDecay, aSustain, aRelease;
    juce::Slider lfoRate, lfoPitch, lfoFilter, glide;

    std::unique_ptr<SliderAttachment> masterAttachment;
    std::array<std::unique_ptr<SliderAttachment>, 3> oscLevelAttachments, oscFineAttachments;
    std::array<std::unique_ptr<ComboAttachment>, 3> oscOctaveAttachments, oscWaveAttachments;
    std::unique_ptr<SliderAttachment> noiseAttachment;
    std::unique_ptr<SliderAttachment> filterCutoffAttachment, filterResAttachment, filterDriveAttachment;
    std::unique_ptr<SliderAttachment> filterEnvAmtAttachment, filterKbAttachment, mixerDriveAttachment;
    std::unique_ptr<SliderAttachment> subOscAttachment;
    std::unique_ptr<ButtonAttachment> noisePinkAttachment;
    std::unique_ptr<ButtonAttachment> mixerOdAttachment;
    std::unique_ptr<ButtonAttachment> filterOdAttachment;
    std::unique_ptr<ComboAttachment> oversampleAttachment;
    std::unique_ptr<SliderAttachment> fAttackAttachment, fDecayAttachment, fSustainAttachment, fReleaseAttachment;
    std::unique_ptr<SliderAttachment> aAttackAttachment, aDecayAttachment, aSustainAttachment, aReleaseAttachment;
    std::unique_ptr<SliderAttachment> lfoRateAttachment, lfoPitchAttachment, lfoFilterAttachment, glideAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MoogVoyagerAudioProcessorEditor)
};
