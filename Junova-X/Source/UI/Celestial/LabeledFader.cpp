#include "LabeledFader.h"

namespace junovax::ui::celestial
{
LabeledFader::LabeledFader (juce::String labelText, juce::Colour labelColour)
    : labelText_ (labelText),
      labelColour_ (labelColour)
{
    label_.setText (labelText_, juce::dontSendNotification);
    label_.setJustificationType (juce::Justification::centred);
    label_.setColour (juce::Label::textColourId, labelColour);
    label_.setVisible (false);

    slider_.setSliderStyle (juce::Slider::LinearVertical);
    slider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 44, 12);
    slider_.setColour (juce::Slider::textBoxTextColourId, Theme::specularChrome());
    slider_.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    slider_.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (slider_);
}

void LabeledFader::paint (juce::Graphics& g)
{
    auto labelArea = getLocalBounds().removeFromTop (16);
    g.setFont (Theme::headerFont (10.5f).boldened());
    g.setColour (labelColour_.withAlpha (1.0f));
    g.drawFittedText (labelText_, labelArea, juce::Justification::centred, 1);
}

void LabeledFader::resized()
{
    slider_.setBounds (getLocalBounds().withTrimmedTop (16));
}
} // namespace junovax::ui::celestial