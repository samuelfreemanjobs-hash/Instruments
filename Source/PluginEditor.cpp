#include "PluginEditor.h"

#include "Assets/FactoryPatchLibrary.h"
#include "Assets/RomFormat.h"

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

    styleRotary (masterGainSlider_, "Master");
    addAndMakeVisible (masterGainSlider_);
    masterGainSlider_.onValueChange = [this]
    {
        if (auto* param = processor_.getAPVTS().getParameter ("masterGain"))
            param->setValueNotifyingHost (static_cast<float> (masterGainSlider_.getValue()));
    };
    if (auto* raw = processor_.getAPVTS().getRawParameterValue ("masterGain"))
        masterGainSlider_.setValue (raw->load(), juce::dontSendNotification);

    styleRotary (filterResonanceSlider_, "Res");
    styleRotary (groupADriveSlider_, "Grp A");
    styleRotary (groupBMixSlider_, "Grp B");
    addAndMakeVisible (filterResonanceSlider_);
    addAndMakeVisible (groupADriveSlider_);
    addAndMakeVisible (groupBMixSlider_);

    filterAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor_.getAPVTS(), "filterResonance", filterResonanceSlider_);
    groupAAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor_.getAPVTS(), "groupADrive", groupADriveSlider_);
    groupBAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor_.getAPVTS(), "groupBMix", groupBMixSlider_);

    couplingCombo_.addItemList ({ "Independent", "Ring 1-2", "Ring 3-4", "X-Mod 1-2", "X-Mod 3-4", "Sync 1-2", "Sync 3-4" }, 1);
    addAndMakeVisible (couplingCombo_);
    couplingAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processor_.getAPVTS(), "couplingMode", couplingCombo_);

    const char* levelIds[4] = { "tone1Level", "tone2Level", "tone3Level", "tone4Level" };
    const char* waveIds[4] = { "tone1Wave", "tone2Wave", "tone3Wave", "tone4Wave" };
    const char* muteIds[4] = { "tone1Mute", "tone2Mute", "tone3Mute", "tone4Mute" };
    const int maxWave = static_cast<int> (jdupgraded::assets::kCleanroomWaveCount) - 1;

    for (int i = 0; i < 4; ++i)
    {
        auto& level = toneLevelSliders_[static_cast<std::size_t> (i)];
        level.setSliderStyle (juce::Slider::LinearVertical);
        level.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 44, 14);
        addAndMakeVisible (level);
        toneLevelAttachments_[static_cast<std::size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                processor_.getAPVTS(), levelIds[i], level);

        auto& wave = toneWaveSliders_[static_cast<std::size_t> (i)];
        wave.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        wave.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 44, 14);
        wave.setRange (0, maxWave, 1.0);
        addAndMakeVisible (wave);
        toneWaveAttachments_[static_cast<std::size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                processor_.getAPVTS(), waveIds[i], wave);

        toneMuteButtons_[static_cast<std::size_t> (i)].setButtonText ("M" + juce::String (i + 1));
        toneMuteButtons_[static_cast<std::size_t> (i)].setClickingTogglesState (true);
        addAndMakeVisible (toneMuteButtons_[static_cast<std::size_t> (i)]);
        toneMuteAttachments_[static_cast<std::size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
                processor_.getAPVTS(), muteIds[i], toneMuteButtons_[static_cast<std::size_t> (i)]);
    }

    startTimerHz (4);
    setSize (680, 440);
}

JDUpgradedAudioProcessorEditor::~JDUpgradedAudioProcessorEditor()
{
    stopTimer();
}

void JDUpgradedAudioProcessorEditor::styleRotary (juce::Slider& slider, const juce::String& name)
{
    slider.setName (name);
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 56, 14);
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
    g.drawFittedText ("Level · Wave · Mute per tone  |  Filter / Group A / B / Coupling",
                      getLocalBounds().removeFromBottom (24),
                      juce::Justification::centred, 1);
}

void JDUpgradedAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (8);
    titleLabel_.setBounds (area.removeFromTop (20));

    auto programRow = area.removeFromTop (26);
    programPrev_.setBounds (programRow.removeFromLeft (32));
    programNext_.setBounds (programRow.removeFromRight (32));
    programLabel_.setBounds (programRow);

    auto fxRow = area.removeFromTop (88);
    const int fxW = fxRow.getWidth() / 5;
    masterGainSlider_.setBounds (fxRow.removeFromLeft (fxW).reduced (4));
    filterResonanceSlider_.setBounds (fxRow.removeFromLeft (fxW).reduced (4));
    groupADriveSlider_.setBounds (fxRow.removeFromLeft (fxW).reduced (4));
    groupBMixSlider_.setBounds (fxRow.removeFromLeft (fxW).reduced (4));
    couplingCombo_.setBounds (fxRow.reduced (4));

    auto toneRow = area;
    const int colW = toneRow.getWidth() / 4;
    for (int i = 0; i < 4; ++i)
    {
        auto col = toneRow.removeFromLeft (colW).reduced (4);
        toneMuteButtons_[static_cast<std::size_t> (i)].setBounds (col.removeFromBottom (22));
        toneWaveSliders_[static_cast<std::size_t> (i)].setBounds (col.removeFromTop (col.getHeight() / 2).reduced (2));
        toneLevelSliders_[static_cast<std::size_t> (i)].setBounds (col.reduced (2));
    }
}
