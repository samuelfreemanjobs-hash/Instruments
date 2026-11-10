#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
class TrapLordSynthAudioProcessorEditor final:public juce::AudioProcessorEditor {
public:
    explicit TrapLordSynthAudioProcessorEditor(TrapLordSynthAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    TrapLordSynthAudioProcessor& processor;
    using SA=juce::AudioProcessorValueTreeState::SliderAttachment;
    using CA=juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using BA=juce::AudioProcessorValueTreeState::ButtonAttachment;
    static constexpr int count=12;
    std::array<juce::Slider,count> knobs;
    std::array<juce::Label,count> labels;
    std::array<std::unique_ptr<SA>,count> attachments;
    juce::ComboBox engine,chord,presets;
    std::unique_ptr<CA> engineAttachment,chordAttachment;
    juce::ToggleButton arp{"ARP ON"};std::unique_ptr<BA> arpAttachment;
    juce::TextButton load{"LOAD SAMPLE MAP"},exportButton{"EXPORT WAV KEYGROUP"};
    juce::Label status;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrapLordSynthAudioProcessorEditor)
};
