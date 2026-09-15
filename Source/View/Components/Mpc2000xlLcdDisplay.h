#pragma once

#include <JuceHeader.h>

namespace vmpc::view
{
/** Waveform + monospace LCD region (Phase 2 custom paint). */
class Mpc2000xlLcdDisplay : public juce::Component
{
public:
    Mpc2000xlLcdDisplay();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setStatusLine(const juce::String& line);
    void setWaveformPreview(const juce::AudioBuffer<float>& buffer);

private:
    juce::String statusLine { "VMPC2000XL — READY" };
    juce::AudioBuffer<float> preview;
    juce::CriticalSection previewLock;
};
} // namespace vmpc::view
