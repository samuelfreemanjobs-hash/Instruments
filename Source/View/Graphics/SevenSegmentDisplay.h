#pragma once

#include <JuceHeader.h>

namespace vmpc::view
{
/** 7-segment LED style readout for BPM, bar, step (Phase 2). */
class SevenSegmentDisplay : public juce::Component
{
public:
    void setText(const juce::String& text);
    void paint(juce::Graphics& g) override;

private:
    void drawDigit(juce::Graphics& g, juce::Rectangle<float> area, juce::juce_wchar c) const;

    juce::String displayText { "120" };
};
} // namespace vmpc::view
