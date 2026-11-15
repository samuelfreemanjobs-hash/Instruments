#pragma once

#include "CelestialTheme.h"

#include <JuceHeader.h>

namespace junovax::ui::celestial
{
/** Vertical fader with micro-cap label above (Sam layout). */
class LabeledFader final : public juce::Component
{
public:
    LabeledFader (juce::String labelText, juce::Colour labelColour);

    juce::Slider& getSlider() noexcept { return slider_; }

    void resized() override;
    void paint (juce::Graphics& g) override;

private:
    juce::String labelText_;
    juce::Colour labelColour_;
    juce::Label label_;
    juce::Slider slider_;
};
} // namespace junovax::ui::celestial