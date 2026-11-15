#pragma once

#include "CelestialTheme.h"

#include <JuceHeader.h>

namespace junovax::ui::celestial
{
class PeakMeterComponent final : public juce::Component
{
public:
    void setLevel (float peak) noexcept
    {
        if (level_ == peak)
            return;
        level_ = peak;
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        auto meter = getLocalBounds().toFloat();
        g.setColour (Theme::ledInactive());
        g.fillRoundedRectangle (meter, 4.0f);
        const float h = meter.getHeight() * juce::jlimit (0.0f, 1.0f, level_ * 4.0f);
        g.setColour (Theme::electricCyan());
        g.fillRoundedRectangle (meter.getX() + 4.0f, meter.getBottom() - h, meter.getWidth() - 8.0f, h, 3.0f);
    }

private:
    float level_ = 0.0f;
};
} // namespace junovax::ui::celestial