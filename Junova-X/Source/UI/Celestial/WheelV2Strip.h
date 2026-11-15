#pragma once

#include "CelestialTheme.h"

#include <JuceHeader.h>

namespace junovax::ui::celestial
{
/** Decorative pitch/mod wheel (engine routing deferred — v2). */
class WheelV2Strip final : public juce::Component
{
public:
    enum class Kind { pitch, mod };

    explicit WheelV2Strip (Kind kind);

    void paint (juce::Graphics& g) override;

private:
    Kind kind_;
};
} // namespace junovax::ui::celestial