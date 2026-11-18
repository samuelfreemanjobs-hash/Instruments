#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
class DrumsmithEditor final : public juce::AudioProcessorEditor {
public:
    explicit DrumsmithEditor(DrumsmithProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    DrumsmithProcessor& processor;
    juce::ComboBox kind;
    juce::Label heading;
    std::array<juce::Slider,6> knobs;
    std::array<juce::Label,6> labels;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> kindAttach;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>,6> attaches;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumsmithEditor)
};
