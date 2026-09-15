#include "PluginEditor.h"

JDUpgradedAudioProcessorEditor::JDUpgradedAudioProcessorEditor (JDUpgradedAudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    titleLabel_.setText ("JD Upgraded — Phase 1 pipeline", juce::dontSendNotification);
    titleLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel_);

    masterGainSlider_.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    masterGainSlider_.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 18);
    addAndMakeVisible (masterGainSlider_);

    masterGainSlider_.onValueChange = [this]
    {
        if (auto* param = processor_.getAPVTS().getParameter ("masterGain"))
            param->setValueNotifyingHost (static_cast<float> (masterGainSlider_.getValue()));
    };

    if (auto* raw = processor_.getAPVTS().getRawParameterValue ("masterGain"))
        masterGainSlider_.setValue (raw->load(), juce::dontSendNotification);

    setSize (480, 240);
}

void JDUpgradedAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a22));
    g.setColour (juce::Colours::white.withAlpha (0.9f));
    g.drawFittedText ("Four tones per voice · 32-sample control rate · zero heap in audio",
                      getLocalBounds().removeFromBottom (36),
                      juce::Justification::centred, 1);
}

void JDUpgradedAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (12);
    titleLabel_.setBounds (area.removeFromTop (28));
    masterGainSlider_.setBounds (area.withSizeKeepingCentre (120, 120));
}
