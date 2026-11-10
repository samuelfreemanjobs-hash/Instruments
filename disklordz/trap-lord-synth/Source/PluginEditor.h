#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "KeygroupExportWorker.h"

class TrapLordSynthAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit TrapLordSynthAudioProcessorEditor (TrapLordSynthAudioProcessor&);
    ~TrapLordSynthAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void selectCategoryAndShowFirst (const juce::String& category);

private:
    void maybeRunScreenshotCapture();
    void timerCallback() override;
    void refreshFilteredIndices();
    void showPresetAtListIndex (int listIndex);
    void startMpcExport();

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
    juce::ComboBox engine, chord, bankMode, categoryFilter;
    juce::TextEditor searchBox;
    juce::Label presetTitle;
    juce::TextButton prevBtn { "<" }, nextBtn { ">" }, exportMpc { "EXPORT KEYGROUP" };
    std::unique_ptr<CA> engineAttachment, chordAttachment, bankAttachment;
    juce::ToggleButton arp { "ARP ON" };
    std::unique_ptr<BA> arpAttachment;
    juce::TextButton load { "LOAD SAMPLE MAP" };
    juce::Label status;
    juce::ProgressBar exportProgress { progressValue };
    double progressValue = 0.;
    juce::Array<int> filteredPresetIndices;
    int listIndex = 0;
    std::unique_ptr<KeygroupExportWorker> exportWorker;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrapLordSynthAudioProcessorEditor)
};
