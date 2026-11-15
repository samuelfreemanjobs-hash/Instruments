#include "UI/Wave9090LookAndFeel.h"

namespace wave9090::ui
{

Wave9090LookAndFeel::Wave9090LookAndFeel()
{
    setColour (juce::Slider::thumbColourId, accent_);
    setColour (juce::Slider::trackColourId, juce::Colour (0xff2a2a36));
    setColour (juce::Slider::backgroundColourId, panelFill_);
    setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff1e1e28));
    setColour (juce::ComboBox::textColourId, textPrimary_);
    setColour (juce::ComboBox::outlineColourId, juce::Colour (0xff3a3a48));
    setColour (juce::Label::textColourId, textMuted_);
    setColour (juce::ToggleButton::textColourId, textPrimary_);
    setColour (juce::TextButton::buttonColourId, juce::Colour (0xff222230));
    setColour (juce::TextButton::textColourOffId, textPrimary_);

    panelBg_ = SkinAssets::loadPanelBackground();
    knobDial_ = SkinAssets::loadKnobDial();
}

void Wave9090LookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPos, float startAngle, float endAngle, juce::Slider& slider)
{
    juce::ignoreUnused (slider);
    const auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (6.0f);
    const float angle = startAngle + sliderPos * (endAngle - startAngle);
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;

    g.setColour (panelFill_);
    g.fillEllipse (bounds);

    if (knobDial_.isValid())
        g.drawImage (knobDial_, bounds, juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);

    juce::Path p;
    p.addRectangle (-2.0f, -radius + 8.0f, 4.0f, radius * 0.42f);
    g.setColour (accent_);
    g.fillPath (p, juce::AffineTransform::rotation (angle).translated (bounds.getCentreX(), bounds.getCentreY()));
}

void Wave9090LookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                             bool highlighted, bool down)
{
    juce::ignoreUnused (highlighted, down);
    auto area = button.getLocalBounds().toFloat().reduced (1.0f);
    g.setColour (button.getToggleState() ? accent_.withAlpha (0.35f) : juce::Colour (0xff222230));
    g.fillRoundedRectangle (area, 6.0f);
    g.setColour (button.getToggleState() ? accent_ : textMuted_);
    g.drawRoundedRectangle (area, 6.0f, 1.0f);
    g.setColour (textPrimary_);
    g.setFont (juce::Font (12.0f));
    g.drawText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred);
}

void Wave9090LookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                                         int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box)
{
    juce::ignoreUnused (isButtonDown, buttonX, buttonY, buttonW, buttonH, box);
    g.setColour (findColour (juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle (0, 0, static_cast<float> (width), static_cast<float> (height), 6.0f);
    g.setColour (findColour (juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle (0.5f, 0.5f, static_cast<float> (width) - 1.0f, static_cast<float> (height) - 1.0f, 6.0f, 1.0f);
}

juce::Font Wave9090LookAndFeel::getLabelFont (juce::Label&)
{
    return juce::Font (11.0f);
}

} // namespace wave9090::ui