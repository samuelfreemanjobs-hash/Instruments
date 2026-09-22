#include "PluginEditor.h"

Rev2TrapAudioProcessorEditor::Rev2TrapAudioProcessorEditor (Rev2TrapAudioProcessor& p)
    : AudioProcessorEditor (p), processor_ (p)
{
    setSize (480, 120);
}

void Rev2TrapAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff121018));
    g.setColour (juce::Colours::white);
    g.setFont (18.0f);
    g.drawText ("Rev2 Trap — Prophet-vibe trap synth", getLocalBounds(), juce::Justification::centred);
}

void Rev2TrapAudioProcessorEditor::resized() {}
