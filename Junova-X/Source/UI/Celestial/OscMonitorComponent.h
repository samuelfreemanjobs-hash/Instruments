#pragma once

#include "CelestialTheme.h"

#include <JuceHeader.h>

class JunovaXAudioProcessor;

namespace junovax::ui::celestial
{
class OscMonitorComponent final : public juce::Component, private juce::Timer
{
public:
    explicit OscMonitorComponent (JunovaXAudioProcessor& processor);

    void paint (juce::Graphics& g) override;

private:
    void timerCallback() override;

    JunovaXAudioProcessor& processor_;
    std::array<float, 256> samples_{};
};
} // namespace junovax::ui::celestial
