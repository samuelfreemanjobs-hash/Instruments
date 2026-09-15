#pragma once

#include <JuceHeader.h>

namespace vmpc::view
{
class ValveForceStrip : public juce::Component,
                        private juce::Timer
{
public:
    ValveForceStrip();

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawTube(juce::Graphics& g, juce::Rectangle<float> area) const;
    void drawVu(juce::Graphics& g, juce::Rectangle<float> area, float needleAngle) const;

    juce::Slider driveL;
    juce::Slider driveR;
    float vuL = -0.4f;
    float vuR = -0.35f;
};
} // namespace vmpc::view
