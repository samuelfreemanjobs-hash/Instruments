#include "UI/Wave9090LookAndFeel.h"

namespace wave9090::ui
{

Wave9090LookAndFeel::Wave9090LookAndFeel()
{
    setColour (juce::Slider::thumbColourId, tokens::cyanGlow());
    setColour (juce::Slider::trackColourId, tokens::surfaceInset());
    setColour (juce::Slider::backgroundColourId, tokens::surfacePanel());
    setColour (juce::ComboBox::backgroundColourId, tokens::surfaceInset());
    setColour (juce::ComboBox::textColourId, tokens::textPrimary());
    setColour (juce::ComboBox::outlineColourId, tokens::textDim());
    setColour (juce::Label::textColourId, tokens::textSecondary());
    setColour (juce::ToggleButton::textColourId, tokens::textPrimary());
    setColour (juce::TextButton::buttonColourId, tokens::surfaceInset());
    setColour (juce::TextButton::textColourOffId, tokens::textSecondary());
    setColour (juce::TextButton::textColourOnId, tokens::cyanGlow());

    knobDial_ = SkinAssets::loadKnobDial();
}

tokens::KnobDomain Wave9090LookAndFeel::domainFromSlider (const juce::Slider& slider) noexcept
{
    const auto id = slider.getComponentID();
    if (id == "amber" || id == "filter" || id == "macro")
        return tokens::KnobDomain::amber;
    if (id == "blue")
        return tokens::KnobDomain::blue;
    return tokens::KnobDomain::cyan;
}

void Wave9090LookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, float startAngle, float endAngle, juce::Slider& slider)
{
    const auto accent = tokens::domainColour (domainFromSlider (slider));
    const auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (5.0f);
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const float angle = startAngle + sliderPos * (endAngle - startAngle);

    // Halo arc ~270° (7 o'clock → 5 o'clock)
    juce::Path halo;
    halo.addCentredArc (bounds.getCentreX(), bounds.getCentreY(), radius - 2.0f, radius - 2.0f, 0.0f,
                        juce::MathConstants<float>::pi * 0.83f, juce::MathConstants<float>::pi * 2.33f, true);
    g.setColour (accent.withAlpha (0.14f));
    g.strokePath (halo, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Titanium cap
    juce::ColourGradient cap (juce::Colour (0xff2c313c), bounds.getX(), bounds.getY(),
                              juce::Colour (0xff10141b), bounds.getRight(), bounds.getBottom(), true);
    g.setGradientFill (cap);
    g.fillEllipse (bounds.reduced (radius * 0.08f));
    g.setColour (juce::Colours::black.withAlpha (0.4f));
    g.drawEllipse (bounds.reduced (radius * 0.08f), 1.2f);

    if (knobDial_.isValid())
        g.drawImage (knobDial_, bounds.reduced (radius * 0.14f), juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);

    // Pointer
    juce::Path pointer;
    pointer.addRectangle (-1.5f, -radius + 10.0f, 3.0f, radius * 0.42f);
    g.setColour (accent.withAlpha (0.35f));
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).translated (bounds.getCentreX(), bounds.getCentreY()));
    g.setColour (accent);
    g.fillPath (pointer, juce::AffineTransform::rotation (angle).scaled (0.92f).translated (bounds.getCentreX(), bounds.getCentreY()));
}

void Wave9090LookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                           bool highlighted, bool down)
{
    juce::ignoreUnused (highlighted, down);
    auto area = button.getLocalBounds().toFloat();
    const bool on = button.getToggleState();
    g.setColour (on ? tokens::amberGlow().withAlpha (0.25f) : tokens::surfaceInset());
    g.fillRoundedRectangle (area, 3.0f);
    g.setColour (on ? tokens::amberGlow() : tokens::textDim());
    g.drawRoundedRectangle (area.reduced (0.5f), 3.0f, 1.0f);
    g.setColour (on ? tokens::textPrimary() : tokens::textSecondary());
    g.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::bold)));
    g.drawText (button.getButtonText().toUpperCase(), button.getLocalBounds(), juce::Justification::centred);
}

void Wave9090LookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                                       int, int, int, int, juce::ComboBox& box)
{
    juce::ignoreUnused (isButtonDown, box);
    auto r = juce::Rectangle<float> (0.0f, 0.0f, static_cast<float> (width), static_cast<float> (height));
    g.setColour (tokens::surfaceInset());
    g.fillRoundedRectangle (r, 4.0f);
    g.setColour (tokens::cyanGlow().withAlpha (0.35f));
    g.drawRoundedRectangle (r.reduced (0.5f), 4.0f, 1.0f);
}

void Wave9090LookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                                const juce::Colour&, bool highlighted, bool down)
{
    juce::ignoreUnused (highlighted, down);
    auto r = button.getLocalBounds().toFloat();
    g.setColour (tokens::surfaceInset().brighter (button.isMouseOver() ? 0.12f : 0.0f));
    g.fillRoundedRectangle (r, 3.0f);
    g.setColour (tokens::textDim());
    g.drawRoundedRectangle (r.reduced (0.5f), 3.0f, 1.0f);
}

juce::Font Wave9090LookAndFeel::getLabelFont (juce::Label&)
{
    return juce::Font (juce::FontOptions (10.0f, juce::Font::bold));
}

} // namespace wave9090::ui