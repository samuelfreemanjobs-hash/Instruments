#include "PluginEditor.h"

JDUpgradedAudioProcessorEditor::JDUpgradedAudioProcessorEditor (JDUpgradedAudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    titleLabel_.setText ("JD Upgraded — 128 programs · 4-tone stack", juce::dontSendNotification);
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

    const char* muteParamIds[4] = { "tone1Mute", "tone2Mute", "tone3Mute", "tone4Mute" };
    for (int i = 0; i < 4; ++i)
    {
        toneMuteButtons_[static_cast<std::size_t> (i)].setButtonText ("T" + juce::String (i + 1) + " Mute");
        toneMuteButtons_[static_cast<std::size_t> (i)].setClickingTogglesState (true);
        addAndMakeVisible (toneMuteButtons_[static_cast<std::size_t> (i)]);

        toneMuteAttachments_[static_cast<std::size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
                processor_.getAPVTS(), muteParamIds[i], toneMuteButtons_[static_cast<std::size_t> (i)]);
    }

    setSize (520, 280);
}

void JDUpgradedAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a22));
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.drawFittedText ("Mute toggles silence a tone; level faders are kept for when you unmute.",
                      getLocalBounds().removeFromBottom (32),
                      juce::Justification::centred, 1);
}

void JDUpgradedAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (12);
    titleLabel_.setBounds (area.removeFromTop (24));

    auto muteRow = area.removeFromBottom (36);
    const int muteW = muteRow.getWidth() / 4;
    for (int i = 0; i < 4; ++i)
        toneMuteButtons_[static_cast<std::size_t> (i)].setBounds (muteRow.removeFromLeft (muteW).reduced (4));

    masterGainSlider_.setBounds (area.withSizeKeepingCentre (120, 120));
}
