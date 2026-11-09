#include "RotaryKnob.h"
#include "../FactoryLookAndFeel.h"

namespace fds {
namespace gui {

namespace {
int bodyDiameter (KnobSize s)
{
    switch (s)
    {
        case KnobSize::sm: return size::knob::sm;
        case KnobSize::lg: return size::knob::lg;
        case KnobSize::xl: return size::knob::xl;
        case KnobSize::md:
        default:           return size::knob::md;
    }
}
} // namespace

RotaryKnob::RotaryKnob (const juce::String& legendText, KnobSize sz)
    : knobSize (sz)
{
    legend.setText (applyCase (legendText, typography::style::legend.textCase), juce::dontSendNotification);
    legend.setJustificationType (juce::Justification::centred);
    legend.setColour (juce::Label::textColourId, argb (color::text::secondary));
    legend.setFont (makeFont (typography::style::legend));
    legend.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (legend);

    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
    slider.setRange (0.0, 1.0, 0.0);
    slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f,
                                juce::MathConstants<float>::pi * 2.75f, true);
    slider.setMouseDragSensitivity (interaction::dragPixelsFullRange);
    slider.setSliderSnapsToMousePosition (false);
    slider.addListener (this);
    addAndMakeVisible (slider);

    valueLabel.setJustificationType (juce::Justification::centred);
    valueLabel.setColour (juce::Label::textColourId, argb (color::text::secondary));
    valueLabel.setFont (makeFont (typography::style::value));
    valueLabel.setEditable (true, false, true);
    valueLabel.onTextChange = [this]
    {
        // Click-to-type: parse simple float 0..1 or percent
        auto t = valueLabel.getText().trim().retainCharacters ("0123456789.+-");
        const float v = juce::jlimit (0.0f, 1.0f, t.getFloatValue());
        setNormalisedValue (v);
    };
    addAndMakeVisible (valueLabel);

    setValueText ("0.0");
    setSize (bodyDiameter (knobSize) + 16, bodyDiameter (knobSize) + 40);
}

RotaryKnob::~RotaryKnob()
{
    slider.removeListener (this);
}

void RotaryKnob::setLegend (const juce::String& text)
{
    legend.setText (applyCase (text, typography::style::legend.textCase), juce::dontSendNotification);
}

void RotaryKnob::setValueText (const juce::String& text)
{
    if (! valueLabel.isBeingEdited())
        valueLabel.setText (text, juce::dontSendNotification);
}

void RotaryKnob::setNormalisedValue (float v, juce::NotificationType n)
{
    slider.setValue ((double) juce::jlimit (0.0f, 1.0f, v), n);
}

float RotaryKnob::getNormalisedValue() const
{
    return (float) slider.getValue();
}

void RotaryKnob::setDefaultNormalised (float v)
{
    defaultNorm = juce::jlimit (0.0f, 1.0f, v);
}

void RotaryKnob::setBipolar (bool bipolar, float originNorm)
{
    slider.getProperties().set ("fdsBipolar", bipolar);
    slider.getProperties().set ("fdsOrigin", originNorm);
    slider.repaint();
}

void RotaryKnob::setEnabledState (bool shouldBeEnabled)
{
    setEnabled (shouldBeEnabled);
    slider.setEnabled (shouldBeEnabled);
}

void RotaryKnob::sliderValueChanged (juce::Slider*)
{
    const float v = getNormalisedValue();
    if (valueFormatter)
        setValueText (valueFormatter (v));
    else
        setValueText (juce::String ((int) std::round (v * 100.0f)) + " %");

    valueLabel.setColour (juce::Label::textColourId,
                          slider.isMouseOverOrDragging() ? argb (color::text::primary)
                                                         : argb (color::text::secondary));
    if (onValueChange)
        onValueChange (v);
}

void RotaryKnob::mouseDoubleClick (const juce::MouseEvent& e)
{
    if (slider.getBounds().contains (e.getPosition()))
        setNormalisedValue (defaultNorm);
}

void RotaryKnob::paint (juce::Graphics& g)
{
    juce::ignoreUnused (g);
}

void RotaryKnob::resized()
{
    auto r = getLocalBounds();
    legend.setBounds (r.removeFromTop (14));
    r.removeFromTop (size::legendGap);
    valueLabel.setBounds (r.removeFromBottom (16));
    const int d = bodyDiameter (knobSize);
    slider.setBounds (r.withSizeKeepingCentre (d, d));
}

std::unique_ptr<juce::AccessibilityHandler> RotaryKnob::createAccessibilityHandler()
{
    return std::make_unique<juce::AccessibilityHandler> (
        *this, juce::AccessibilityRole::slider, juce::AccessibilityActions());
}

} // namespace gui
} // namespace fds
