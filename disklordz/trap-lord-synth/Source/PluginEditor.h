#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class TrapLordSynthAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit TrapLordSynthAudioProcessorEditor (TrapLordSynthAudioProcessor&);
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void refreshPresetMenu();

    TrapLordSynthAudioProcessor& processor;
    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using CA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using BA = juce::AudioProcessorValueTreeState::ButtonAttachment;
    static constexpr int count = 12;
    static constexpr int macroCount = 4;
    std::array<juce::Slider, count> knobs;
    std::array<juce::Slider, macroCount> macroKnobs;
    std::array<juce::Label, count> labels;
    std::array<juce::Label, macroCount> macroLabels;
    std::array<std::unique_ptr<SA>, count> attachments;
    std::array<std::unique_ptr<SA>, macroCount> macroAttachments;
    juce::ComboBox engine, chord, presets, bankMode;
    std::unique_ptr<CA> engineAttachment, chordAttachment, bankAttachment;
    juce::ToggleButton arp { "ARP ON" };
    std::unique_ptr<BA> arpAttachment;
    juce::TextButton load { "LOAD SAMPLE MAP" }, exportButton { "EXPORT WAV KEYGROUP" };
    juce::Label status;
    std::vector<int> visiblePresetIndices;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrapLordSynthAudioProcessorEditor)
};
