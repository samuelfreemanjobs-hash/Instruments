#include "PluginEditor.h"

TrapForgeAudioProcessorEditor::TrapForgeAudioProcessorEditor (TrapForgeAudioProcessor& p)
    : AudioProcessorEditor (p),
      processor_ (p)
{
    titleLabel_.setText ("TRAP-FORGE", juce::dontSendNotification);
    titleLabel_.setFont (juce::FontOptions (22.0f, juce::Font::bold));
    titleLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel_);

    presetLabel_.setText ("Factory preset (host program)", juce::dontSendNotification);
    presetLabel_.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (presetLabel_);

    for (int i = 0; i < processor_.getNumPrograms(); ++i)
        programBox_.addItem (processor_.getProgramName (i), i + 1);

    programBox_.setSelectedId (processor_.getCurrentProgram() + 1, juce::dontSendNotification);
    programBox_.onChange = [this]
    {
        processor_.setCurrentProgram (programBox_.getSelectedId() - 1);
        presetLabel_.setText (processor_.getActivePreset().name, juce::dontSendNotification);
    };

    addAndMakeVisible (programBox_);
    presetLabel_.setText (processor_.getActivePreset().name, juce::dontSendNotification);

    setSize (360, 140);
}

TrapForgeAudioProcessorEditor::~TrapForgeAudioProcessorEditor() = default;

void TrapForgeAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff121212));
    g.setColour (juce::Colours::orange);
    g.drawRect (getLocalBounds(), 2);
}

void TrapForgeAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (12);
    titleLabel_.setBounds (area.removeFromTop (32));
    presetLabel_.setBounds (area.removeFromTop (24));
    programBox_.setBounds (area.removeFromTop (28));
}
