#pragma once

#include "DSP/WavetableBank.h"

#include <JuceHeader.h>

namespace wave9090::ui
{

/** VFD-style wavetable preview (UI only). */
class WavetableScope final : public juce::Component,
                             private juce::Timer
{
public:
    WavetableScope();

    void setWavetablePosition (float pos01) noexcept;
    void paint (juce::Graphics&) override;

private:
    void timerCallback() override;
    void ensureBank() const;

    mutable std::unique_ptr<dsp::WavetableBank> bank_;
    float position_ = 0.35f;
    float scanPhase_ = 0.0f;
};

} // namespace wave9090::ui