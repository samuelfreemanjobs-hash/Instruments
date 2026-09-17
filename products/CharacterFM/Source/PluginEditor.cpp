#include "PluginEditor.h"

#include "Parameters/ParameterIds.h"
#include "PluginProcessor.h"

CharacterFMEditor::CharacterFMEditor (CharacterFMAudioProcessor& p)
    : AudioProcessorEditor (p),
      processor_ (p)
{
    titleLabel_.setText ("Character FM (Milestone 1)", juce::dontSendNotification);
    titleLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel_);

    masterSlider_.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    masterSlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 18);
    addAndMakeVisible (masterSlider_);

    algorithmSlider_.setSliderStyle (juce::Slider::LinearHorizontal);
    algorithmSlider_.setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 18);
    addAndMakeVisible (algorithmSlider_);

    algorithmLabel_.setText ("Algorithm", juce::dontSendNotification);
    addAndMakeVisible (algorithmLabel_);

    masterAttachment_ = std::make_unique<SliderAttachment> (processor_.getAPVTS(),
                                                          characterfm::params::masterGain,
                                                          masterSlider_);
    algorithmAttachment_ = std::make_unique<SliderAttachment> (processor_.getAPVTS(),
                                                               characterfm::params::algorithm,
                                                               algorithmSlider_);

    setSize (420, 220);
}

void CharacterFMEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a22));
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.setFont (14.0f);
}

void CharacterFMEditor:: resized()
{
    auto r = getLocalBounds().reduced (12);
    titleLabel_.setBounds (r.removeFromTop (28));
    algorithmLabel_.setBounds (r.removeFromTop (20));
    algorithmSlider_.setBounds (r.removeFromTop (32));
    masterSlider_.setBounds (r.removeFromTop (100).withSizeKeepingCentre (100, 100));
}
