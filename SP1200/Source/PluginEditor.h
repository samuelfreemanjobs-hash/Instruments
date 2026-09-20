#pragma once

#include "PluginProcessor.h"

#include <array>

class SP1200AudioProcessorEditor : public juce::AudioProcessorEditor,
                                   private juce::Timer
{
public:
    explicit SP1200AudioProcessorEditor (SP1200AudioProcessor&);
    ~SP1200AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    enum class ViewMode
    {
        console,
        sequencer,
        song
    };

    void timerCallback() override;
    void refreshMemoryLabel();
    void refreshSeqInfo();
    void triggerPad (int padIndex);
    void importSample();
    void toggleRecordInput();
    void setView (ViewMode mode);
    void layoutConsole (juce::Rectangle<int> area);
    void layoutSequencer (juce::Rectangle<int> area);
    void layoutSong (juce::Rectangle<int> area);
    void runAutoChop16();

    SP1200AudioProcessor& processor_;

    ViewMode view_ = ViewMode::console;

    juce::TextButton consoleTab_ { "10 CONSOLE" };
    juce::TextButton seqTab_ { "MOD 20 SEQ" };
    juce::TextButton songTab_ { "24 SONG" };

    juce::Label headerLabel_;
    juce::Label rateLabel_;
    juce::Label memoryLabel_;
    juce::Label engagedLabel_;

    juce::TextButton importButton_ { "Import WAV" };
    juce::TextButton recordButton_ { "ARM INPUT" };
    juce::TextButton faderModeButton_ { "FADER: VOL" };
    juce::TextButton chopButton_ { "Auto-chop 16 (transient)" };
    juce::ToggleButton multiPitchButton_ { "MULTI-PITCH (16 cap)" };

    juce::Slider patternSlider_;
    juce::Slider barsSlider_;
    juce::Label seqInfoLabel_;
    juce::TextButton seqPlayButton_ { "PLAY PATTERN" };
    juce::TextButton seqStopButton_ { "STOP" };
    juce::ToggleButton seqRecordSteps_ { "Record steps (pads)" };

    juce::Label songInfoLabel_;
    std::array<juce::ComboBox, 8> songSlotBoxes_;
    juce::TextButton songPlayButton_ { "PLAY SONG" };

    std::array<juce::TextButton, sp1200::kNumPads> padButtons_;
    std::array<juce::Slider, sp1200::kNumPads> faders_;

    int faderMode_ = 0;
    int lastSegmentForChop_ = -1;
    bool seqRecordMode_ = false;
};
