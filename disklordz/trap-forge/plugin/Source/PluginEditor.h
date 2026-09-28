#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"

class TrapForgeAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit TrapForgeAudioProcessorEditor (TrapForgeAudioProcessor&);
    ~TrapForgeAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    TrapForgeAudioProcessor& processor_;
    juce::Label titleLabel_;
    juce::Label presetLabel_;
    juce::ComboBox programBox_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrapForgeAudioProcessorEditor)
};
