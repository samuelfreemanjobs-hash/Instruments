#pragma once

#include "UI/DiagPanel.h"
#include "UI/MainPanel.h"

#include <JuceHeader.h>

class JunovaXAudioProcessor;

class JunovaXAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit JunovaXAudioProcessorEditor (JunovaXAudioProcessor&);
    ~JunovaXAudioProcessorEditor() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    JunovaXAudioProcessor& processor_;
    juce::TabbedComponent tabs_ { juce::TabbedButtonBar::TabsAtTop };
    junovax::ui::MainPanel mainPanel_;
    junovax::ui::DiagPanel diagPanel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JunovaXAudioProcessorEditor)
};
