#include "StudioLookAndFeel.h"

namespace resonance::view
{
StudioLookAndFeel::StudioLookAndFeel()
{
    matteBlack = juce::Colour(0xff121214);
    walnutBrown = juce::Colour(0xff3d2b1f);
    iceBlue = juce::Colour(0xff6ec8e8);
    neonAccent = juce::Colour(0xff3fffd6);

    setColour(juce::ResizableWindow::backgroundColourId, matteBlack);
    setColour(juce::Slider::thumbColourId, iceBlue);
    setColour(juce::Slider::trackColourId, walnutBrown);
    setColour(juce::TextButton::buttonColourId, matteBlack.brighter(0.08f));
    setColour(juce::TextButton::textColourOffId, iceBlue);
}

void StudioLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                       int x,
                                       int y,
                                       int width,
                                       int height,
                                       float sliderPosProportional,
                                       float rotaryStartAngle,
                                       float rotaryEndAngle,
                                       juce::Slider& slider)
{
    juce::ignoreUnused(slider);
    const auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
    const float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    const auto centre = bounds.getCentre();

    g.setColour(matteBlack.brighter(0.15f));
    g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

    g.setColour(neonAccent);
    juce::Path p;
    p.addCentredArc(centre.x, centre.y, radius * 0.85f, radius * 0.85f, 0.0f, rotaryStartAngle, angle, true);
    g.strokePath(p, juce::PathStrokeType(2.5f));
}

void StudioLookAndFeel::drawButtonBackground(juce::Graphics& g,
                                           juce::Button& button,
                                           const juce::Colour& backgroundColour,
                                           bool shouldDrawButtonAsHighlighted,
                                           bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused(backgroundColour);
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    const auto base = shouldDrawButtonAsDown ? walnutBrown.brighter(0.2f)
                                             : (shouldDrawButtonAsHighlighted ? walnutBrown.brighter(0.35f) : walnutBrown);
    g.setColour(base);
    g.fillRoundedRectangle(bounds, 3.0f);
    g.setColour(iceBlue.withAlpha(button.getToggleState() ? 0.9f : 0.35f));
    g.drawRoundedRectangle(bounds, 3.0f, 1.0f);
}
} // namespace resonance::view
