#pragma once

#include "PluginProcessor.h"

#include <array>

class ChordEngineAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                              private juce::Timer
{
public:
    explicit ChordEngineAudioProcessorEditor(ChordEngineAudioProcessor&);
    ~ChordEngineAudioProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void refreshChordLabels();

    ChordEngineAudioProcessor& processorRef;

    juce::ComboBox keyBox;
    juce::ComboBox scaleBox;
    juce::ComboBox presetBox;
    juce::Slider spreadSlider;
    juce::Slider inversionSlider;
    juce::ToggleButton hostMidiToggle;
    juce::ToggleButton auditionToggle;
    juce::TextButton auditionButton { "Audition" };
    juce::Label helpLabel;

    std::array<juce::Label, ChordEngineAudioProcessor::kProgressionLength> chordLabels;
    std::array<juce::TextButton, ChordEngineAudioProcessor::kProgressionLength> stepButtons;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> rootAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> scaleAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> presetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spreadAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inversionAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> hostMidiAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> auditionAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChordEngineAudioProcessorEditor)
};