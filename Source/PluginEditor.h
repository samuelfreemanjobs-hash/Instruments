#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "PluginProcessor.h"

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

    JDUpgradedAudioProcessor& processor_;
    juce::Slider masterGainSlider_;
    juce::Slider filterResonanceSlider_;
    juce::Slider groupADriveSlider_;
    juce::Slider groupBMixSlider_;
    juce::ComboBox couplingCombo_;
    juce::Label titleLabel_;
    juce::Label programLabel_;
    juce::TextButton programPrev_ { "<" };
    juce::TextButton programNext_ { ">" };
    std::array<juce::Slider, 4> toneLevelSliders_;
    std::array<juce::Slider, 4> toneWaveSliders_;
    std::array<juce::Slider, 4> toneMultisampleSliders_;
    std::array<juce::ToggleButton, 4> toneMuteButtons_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> groupAAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> groupBAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> couplingAttachment_;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 4> toneLevelAttachments_;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 4> toneWaveAttachments_;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 4> toneMsAttachments_;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, 4> toneMuteAttachments_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JDUpgradedAudioProcessorEditor)
};
