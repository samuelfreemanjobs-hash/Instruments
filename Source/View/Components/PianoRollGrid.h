#pragma once

#include <JuceHeader.h>

namespace vmpc::view
{
/** Piano roll placeholder — note grid + keys (Phase 2). */
class PianoRollGrid : public juce::Component
{
public:
    PianoRollGrid();

    void paint(juce::Graphics& g) override;

private:
    static constexpr int kVisibleKeys = 24;
};
} // namespace vmpc::view
