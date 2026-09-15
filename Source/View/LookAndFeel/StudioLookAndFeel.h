#pragma once

#include <JuceHeader.h>

namespace resonance::view
{
/** Matte black, walnut accents, ice-blue/neon highlights (Phase 2 theming). */
class StudioLookAndFeel : public juce::LookAndFeel_V4
{
public:
    StudioLookAndFeel();

    void drawRotarySlider(juce::Graphics& g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPosProportional,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider& slider) override;

    void drawButtonBackground(juce::Graphics& g,
                              juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    juce::Colour matteBlack;
    juce::Colour walnutBrown;
    juce::Colour iceBlue;
    juce::Colour neonAccent;
};
} // namespace resonance::view
