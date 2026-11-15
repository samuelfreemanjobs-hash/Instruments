#pragma once

#include <JuceHeader.h>

namespace wave9090::ui
{

/** Segmented LED meter (green / yellow / red). */
class LedOutputMeter final : public juce::Component
{
public:
    void setLevelDb (float peakDb) noexcept;

    void paint (juce::Graphics&) override;

private:
    float peakDb_ = -60.0f;
};

} // namespace wave9090::ui