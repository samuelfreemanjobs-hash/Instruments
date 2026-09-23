#pragma once

#include "PluginProcessor.h"

class Rev2TrapAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit Rev2TrapAudioProcessorEditor (Rev2TrapAudioProcessor&);
    ~Rev2TrapAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    Rev2TrapAudioProcessor& processor_;
    juce::Slider trapMacro_, wtBlend_, cutoff_, drive_;
    juce::Label presetLabel_;
    using Att = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Att> trapAtt_, wtAtt_, cutoffAtt_, driveAtt_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Rev2TrapAudioProcessorEditor)
};
