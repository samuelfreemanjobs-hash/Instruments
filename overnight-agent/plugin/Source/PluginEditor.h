#pragma once
#include "PluginProcessor.h"

class OvernightDrumAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit OvernightDrumAudioProcessorEditor(OvernightDrumAudioProcessor&);
    ~OvernightDrumAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    OvernightDrumAudioProcessor& proc_;
    juce::MidiKeyboardComponent keyboard_;
    juce::TextButton reloadBtn_ { "Reload best_kit" };
    juce::Label status_;
    juce::Slider tune_, decay_, gain_;
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> tuneAtt_, decayAtt_, gainAtt_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OvernightDrumAudioProcessorEditor)
};
