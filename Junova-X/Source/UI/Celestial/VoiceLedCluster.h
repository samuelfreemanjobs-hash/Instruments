#pragma once

#include "CelestialTheme.h"

#include <JuceHeader.h>

class JunovaXAudioProcessor;

namespace junovax::ui::celestial
{
class VoiceLedCluster final : public juce::Component
{
public:
    explicit VoiceLedCluster (JunovaXAudioProcessor& processor);

    void setActiveMask (std::uint8_t mask) noexcept { activeMask_ = mask; repaint(); }

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    JunovaXAudioProcessor& processor_;
    std::uint8_t activeMask_ = 0;
};
} // namespace junovax::ui::celestial