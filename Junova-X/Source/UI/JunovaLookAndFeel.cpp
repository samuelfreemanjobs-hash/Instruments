#include "JunovaLookAndFeel.h"

#include "UI/Celestial/CelestialTheme.h"

namespace junovax::ui
{
namespace
{
using Theme = celestial::Theme;
}

JunovaLookAndFeel::JunovaLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, Theme::background());
    setColour (juce::Slider::trackColourId, Theme::ledInactive());
    setColour (juce::Slider::thumbColourId, Theme::specularChrome());
    setColour (juce::Slider::backgroundColourId, Theme::panelGrey());
    setColour (juce::Slider::textBoxTextColourId, Theme::specularChrome());
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Label::textColourId, Theme::specularChrome());
    setColour (juce::TextButton::buttonColourId, Theme::ledInactive());
    setColour (juce::TextButton::textColourOffId, Theme::textMuted());
    setColour (juce::TextButton::textColourOnId, Theme::electricCyan());
    setColour (juce::ComboBox::backgroundColourId, Theme::panelGrey());
    setColour (juce::PopupMenu::backgroundColourId, Theme::panelGrey());
}

void JunovaLookAndFeel::tagSlider (juce::Slider& slider, Accent accent) noexcept
{
    slider.getProperties().set (kAccentProperty, static_cast<int> (accent));
}

JunovaLookAndFeel::Accent JunovaLookAndFeel::accentFor (const juce::Slider& slider) noexcept
{
    const int v = slider.getProperties().getWithDefault (kAccentProperty, static_cast<int> (Accent::cyan));
    return static_cast<Accent> (juce::jlimit (0, 2, v));
}

juce::Colour JunovaLookAndFeel::colourForAccent (Accent accent) noexcept
{
    switch (accent)
    {
        case Accent::crimson:
            return Theme::stellarCrimson();
        case Accent::chrome:
            return Theme::specularChrome();
        case Accent::cyan:
        default:
            return Theme::electricCyan();
    }
}

void JunovaLookAndFeel::drawLinearSlider (juce::Graphics& g,
                                          int x,
                                          int y,
                                          int width,
                                          int height,
                                          float sliderPos,
                                          float,
                                          float,
                                          juce::Slider::SliderStyle,
                                          juce::Slider& slider)
{
    const auto accent = colourForAccent (accentFor (slider));
    auto track = juce::Rectangle<float> (static_cast<float> (x + width / 2 - 3),
                                         static_cast<float> (y + 6),
                                         6.0f,
                                         static_cast<float> (height - 12));
    g.setColour (Theme::ledInactive());
    g.fillRoundedRectangle (track, 3.0f);

    const float fillH = track.getBottom() - sliderPos;
    juce::Rectangle<float> glow (track.getX() + 1.0f, sliderPos, track.getWidth() - 2.0f, fillH);
    g.setColour (accent.withAlpha (0.25f));
    g.fillRoundedRectangle (glow, 2.0f);
    g.setColour (accent);
    g.fillRoundedRectangle (track.getX(), sliderPos - 1.0f, track.getWidth(), 2.0f, 1.0f);

  #if ! JUCE_MINGW
    juce::Rectangle<float> cap (track.getX() - 5.0f, sliderPos - 7.0f, track.getWidth() + 10.0f, 14.0f);
    g.setColour (Theme::specularChrome());
    g.fillRoundedRectangle (cap, 3.0f);
    g.setColour (accent.withAlpha (0.6f));
    g.drawRoundedRectangle (cap, 3.0f, 1.0f);
  #endif
}

void JunovaLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                          int x,
                                          int y,
                                          int width,
                                          int height,
                                          float sliderPosProportional,
                                          float rotaryStartAngle,
                                          float rotaryEndAngle,
                                          juce::Slider& slider)
{
    const auto accent = colourForAccent (accentFor (slider));
    const float radius = juce::jmin (width, height) * 0.5f - 4.0f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    g.setColour (Theme::ledInactive());
    g.fillEllipse (centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

    juce::Path arc;
    arc.addCentredArc (centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (accent.withAlpha (0.35f));
    g.strokePath (arc, juce::PathStrokeType (3.0f));

    juce::Path indicator;
    const float dotR = 3.0f;
    const float ix = centreX + std::sin (angle) * (radius - 6.0f);
    const float iy = centreY - std::cos (angle) * (radius - 6.0f);
    indicator.addEllipse (ix - dotR, iy - dotR, dotR * 2.0f, dotR * 2.0f);
    g.setColour (Theme::specularChrome());
    g.fillPath (indicator);

    g.setColour (accent.withAlpha (0.5f));
    g.drawEllipse (centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f, 1.2f);

    juce::ignoreUnused (slider);
}

void JunovaLookAndFeel::drawToggleButton (juce::Graphics& g,
                                         juce::ToggleButton& button,
                                         bool,
                                         bool)
{
    auto b = button.getLocalBounds().toFloat().reduced (1.0f);
    const bool on = button.getToggleState();
    g.setColour (on ? Theme::electricCyan().withAlpha (0.18f) : Theme::ledInactive());
    g.fillRoundedRectangle (b, 5.0f);
    g.setColour (on ? Theme::electricCyan() : Theme::textMuted());
    g.drawRoundedRectangle (b, 5.0f, on ? 1.5f : 1.0f);
    if (on)
    {
        g.setColour (Theme::electricCyan());
        g.fillRect (b.getRight() - 4.0f, b.getY() + 4.0f, 2.0f, b.getHeight() - 8.0f);
    }
    g.setColour (on ? Theme::specularChrome() : Theme::textMuted());
    g.setFont (Theme::labelFont());
    g.drawFittedText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, 1);
}

void JunovaLookAndFeel::drawButtonBackground (juce::Graphics& g,
                                              juce::Button& button,
                                              const juce::Colour&,
                                              bool highlighted,
                                              bool down)
{
    auto b = button.getLocalBounds().toFloat();
    const bool on = button.getToggleState();
    auto fill = on ? Theme::electricCyan().withAlpha (0.22f) : Theme::ledInactive();
    if (highlighted)
        fill = fill.brighter (0.12f);
    if (down)
        fill = fill.darker (0.1f);
    g.setColour (fill);
    g.fillRoundedRectangle (b.reduced (0.5f), 5.0f);
    g.setColour (on ? Theme::electricCyan() : Theme::textMuted().withAlpha (0.6f));
    g.drawRoundedRectangle (b.reduced (0.5f), 5.0f, on ? 1.5f : 1.0f);
}

juce::Font JunovaLookAndFeel::getLabelFont (juce::Label& label)
{
    juce::ignoreUnused (label);
    return Theme::labelFont();
}
} // namespace junovax::ui