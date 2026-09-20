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
    void timerCallback() override;
    void refreshMemoryLabel();
    void triggerPad (int padIndex);
    void importSample();
    void toggleRecordInput();

    SP1200AudioProcessor& processor_;

    juce::Label headerLabel_;
    juce::Label rateLabel_;
    juce::Label memoryLabel_;
    juce::Label engagedLabel_;

    juce::TextButton importButton_ { "Import WAV" };
    juce::TextButton recordButton_ { "ARM INPUT" };
    juce::TextButton faderModeButton_ { "FADER: VOL" };

    std::array<juce::TextButton, sp1200::kNumPads> padButtons_;
    std::array<juce::Slider, sp1200::kNumPads> faders_;

    int faderMode_ = 0;
};
