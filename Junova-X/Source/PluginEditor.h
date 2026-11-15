#pragma once

#include "UI/Celestial/CelestialMainPanel.h"
#include "UI/Celestial/PresetVaultBrowser.h"
#include "UI/DiagPanel.h"
#include "UI/JunovaLookAndFeel.h"

#include <JuceHeader.h>

class JunovaXAudioProcessor;

class JunovaXAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit JunovaXAudioProcessorEditor (JunovaXAudioProcessor&);
    ~JunovaXAudioProcessorEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void syncForSnapshot();

private:
    JunovaXAudioProcessor& processor_;
    junovax::ui::JunovaLookAndFeel skin_;
    junovax::ui::celestial::CelestialMainPanel celestialPanel_;
    junovax::ui::celestial::PresetVaultBrowser vault_;
    junovax::ui::DiagPanel diagPanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JunovaXAudioProcessorEditor)
};