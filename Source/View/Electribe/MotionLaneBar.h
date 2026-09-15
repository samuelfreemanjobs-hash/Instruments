#pragma once

#include <JuceHeader.h>

namespace vmpc::view
{
class MotionLaneBar : public juce::Component
{
public:
    void paint(juce::Graphics& g) override;
};
} // namespace vmpc::view
