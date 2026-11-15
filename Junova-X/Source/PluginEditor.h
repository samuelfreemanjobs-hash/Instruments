#pragma once

#include "UI/Celestial/CelestialMainPanel.h"
#include "UI/Celestial/PresetVaultBrowser.h"
#include "UI/DiagPanel.h"
#include "UI/JunovaLookAndFeel.h"

#include <JuceHeader.h>

class JunovaXAudioProcessor;

class JunovaXAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                           private juce::Timer
{
public:
    explicit JunovaXAudioProcessorEditor (JunovaXAudioProcessor&);
    ~JunovaXAudioProcessorEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void applyZoom();

    JunovaXAudioProcessor& processor_;
    junovax::ui::JunovaLookAndFeel skin_;
    junovax::ui::celestial::CelestialMainPanel celestialPanel_;
    junovax::ui::celestial::PresetVaultBrowser vault_;
    junovax::ui::DiagPanel diagPanel_;

    std::unique_ptr<juce::MidiKeyboardComponent> keyboard_;
    juce::Slider pitchWheel_;
    juce::Slider modWheel_;
    juce::TextButton transposeDown_ { "-1 OCT" };
    juce::TextButton transposeZero_ { "0" };
    juce::TextButton transposeUp_ { "+1 OCT" };
    int transposeOctaves_ = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JunovaXAudioProcessorEditor)
};