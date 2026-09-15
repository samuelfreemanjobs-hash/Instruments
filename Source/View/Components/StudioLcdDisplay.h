#pragma once

#include <JuceHeader.h>

namespace resonance::view
{
/** Waveform + monospace LCD region (Phase 2 custom paint). */
class StudioLcdDisplay : public juce::Component
{
public:
    StudioLcdDisplay();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setStatusLine(const juce::String& line);
    void setWaveformPreview(const juce::AudioBuffer<float>& buffer);

private:
    juce::String statusLine { "Resonance — READY" };
    juce::AudioBuffer<float> preview;
    juce::CriticalSection previewLock;
};
} // namespace resonance::view
