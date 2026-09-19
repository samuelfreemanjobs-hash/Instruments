#pragma once

#include "SynthProcessor.h"
#include <JuceHeader.h>
#include <array>

class MoogVoyagerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit MoogVoyagerAudioProcessorEditor(MoogVoyagerAudioProcessor&);
    ~MoogVoyagerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    static void styleKnob(juce::Slider& s, const juce::String& name);
    static void bindCombo(juce::ComboBox& box, const juce::StringArray& items);
    static void sectionLabel(juce::Label& label, const juce::String& text);

    MoogVoyagerAudioProcessor& processorRef;

    juce::Label titleLabel;
    juce::Label oscSectionLabel, filterSectionLabel, envSectionLabel, modSectionLabel;

    juce::Slider masterSlider;
    juce::Slider oscLevel[3], oscFine[3], noiseSlider;
    juce::ComboBox oscOctave[3], oscWave[3];
    juce::Label oscIndexLabel[3], oscOctLabel[3], oscWaveLabel[3];

    juce::Slider filterCutoff, filterRes, filterDrive, filterEnvAmt, filterKb;
    juce::Slider fAttack, fDecay, fSustain, fRelease;
    juce::Slider aAttack, aDecay, aSustain, aRelease;
    juce::Slider lfoRate, lfoPitch, lfoFilter, glide;

    std::unique_ptr<SliderAttachment> masterAttachment;
    std::array<std::unique_ptr<SliderAttachment>, 3> oscLevelAttachments, oscFineAttachments;
    std::array<std::unique_ptr<ComboAttachment>, 3> oscOctaveAttachments, oscWaveAttachments;
    std::unique_ptr<SliderAttachment> noiseAttachment;
    std::unique_ptr<SliderAttachment> filterCutoffAttachment, filterResAttachment, filterDriveAttachment;
    std::unique_ptr<SliderAttachment> filterEnvAmtAttachment, filterKbAttachment;
    std::unique_ptr<SliderAttachment> fAttackAttachment, fDecayAttachment, fSustainAttachment, fReleaseAttachment;
    std::unique_ptr<SliderAttachment> aAttackAttachment, aDecayAttachment, aSustainAttachment, aReleaseAttachment;
    std::unique_ptr<SliderAttachment> lfoRateAttachment, lfoPitchAttachment, lfoFilterAttachment, glideAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MoogVoyagerAudioProcessorEditor)
};
