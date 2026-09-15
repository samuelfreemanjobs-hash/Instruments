#include "PluginEditor.h"

#include "Assets/FactoryPatchLibrary.h"

JDUpgradedAudioProcessorEditor::JDUpgradedAudioProcessorEditor (JDUpgradedAudioProcessor& p)
    : AudioProcessorEditor (&p), processor_ (p)
{
    titleLabel_.setText ("JD Upgraded", juce::dontSendNotification);
    titleLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel_);

    programLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (programLabel_);

    programPrev_.onClick = [this] { changeProgramByDelta (-1); };
    programNext_.onClick = [this] { changeProgramByDelta (1); };
    addAndMakeVisible (programPrev_);
    addAndMakeVisible (programNext_);

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

    const char* levelIds[4] = { "tone1Level", "tone2Level", "tone3Level", "tone4Level" };
    const char* muteIds[4] = { "tone1Mute", "tone2Mute", "tone3Mute", "tone4Mute" };

    for (int i = 0; i < 4; ++i)
    {
        auto& slider = toneLevelSliders_[static_cast<std::size_t> (i)];
        slider.setSliderStyle (juce::Slider::LinearVertical);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 48, 16);
        slider.setName ("Tone " + juce::String (i + 1));
        addAndMakeVisible (slider);

        toneLevelAttachments_[static_cast<std::size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                processor_.getAPVTS(), levelIds[i], slider);

        toneMuteButtons_[static_cast<std::size_t> (i)].setButtonText ("M" + juce::String (i + 1));
        toneMuteButtons_[static_cast<std::size_t> (i)].setClickingTogglesState (true);
        addAndMakeVisible (toneMuteButtons_[static_cast<std::size_t> (i)]);

        toneMuteAttachments_[static_cast<std::size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
                processor_.getAPVTS(), muteIds[i], toneMuteButtons_[static_cast<std::size_t> (i)]);
    }

    startTimerHz (4);
    setSize (560, 360);
}

JDUpgradedAudioProcessorEditor::~JDUpgradedAudioProcessorEditor()
{
    stopTimer();
}

void JDUpgradedAudioProcessorEditor::changeProgramByDelta (int delta)
{
    const int count = static_cast<int> (jdupgraded::assets::FactoryPatchLibrary::getPatchCount());
    int program = processor_.getCurrentProgram() + delta;
    program = juce::jlimit (0, count - 1, program);
    processor_.setCurrentProgram (program);
}

void JDUpgradedAudioProcessorEditor::timerCallback()
{
    const int program = processor_.getCurrentProgram();
    const auto& patch = jdupgraded::assets::FactoryPatchLibrary::getPatch (static_cast<std::size_t> (program));
    programLabel_.setText (juce::String (program + 1) + " / "
                           + juce::String (jdupgraded::assets::FactoryPatchLibrary::getPatchCount())
                           + "  " + patch.name,
                           juce::dontSendNotification);
}

void JDUpgradedAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a22));
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.drawFittedText ("Four-tone stack · mute M1–M4 silences a layer without changing level",
                      getLocalBounds().removeFromBottom (28),
                      juce::Justification::centred, 1);
}

void JDUpgradedAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (10);
    titleLabel_.setBounds (area.removeFromTop (22));

    auto programRow = area.removeFromTop (28);
    programPrev_.setBounds (programRow.removeFromLeft (36));
    programNext_.setBounds (programRow.removeFromRight (36));
    programLabel_.setBounds (programRow);

    auto bottom = area.removeFromBottom (140);
    const int colW = bottom.getWidth() / 5;
    masterGainSlider_.setBounds (bottom.removeFromLeft (colW).reduced (6));

    for (int i = 0; i < 4; ++i)
    {
        auto col = bottom.removeFromLeft (colW).reduced (4);
        toneMuteButtons_[static_cast<std::size_t> (i)].setBounds (col.removeFromBottom (24));
        toneLevelSliders_[static_cast<std::size_t> (i)].setBounds (col);
    }
}
