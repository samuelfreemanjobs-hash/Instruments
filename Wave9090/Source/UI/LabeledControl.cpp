#include "UI/LabeledControl.h"

namespace wave9090::ui
{

LabeledKnob::LabeledKnob (juce::String legendText)
    : legend_ ({}, legendText)
{
    legend_.setJustificationType (juce::Justification::centred);
    legend_.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (legend_);

    slider_.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider_.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider_.setPopupDisplayEnabled (false, false, nullptr);
    slider_.onValueChange = [this] { sliderValueChanged(); };
    addAndMakeVisible (slider_);

    value_.setJustificationType (juce::Justification::centred);
    value_.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (value_);
}

void LabeledKnob::resized()
{
    auto area = getLocalBounds();
    legend_.setBounds (area.removeFromTop (16));
    value_.setBounds (area.removeFromBottom (16));
    slider_.setBounds (area);
}

void LabeledKnob::setValueText (const juce::String& text)
{
    value_.setText (text, juce::dontSendNotification);
}

void LabeledKnob::sliderValueChanged()
{
    if (formatValue)
        setValueText (formatValue (static_cast<float> (slider_.getValue())));
}

} // namespace wave9090::ui