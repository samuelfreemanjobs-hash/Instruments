#include "PluginEditor.h"
#include "Parameters/ParameterIds.h"

namespace
{
using PID = rev2trap::ParameterIDs;

void styleSlider (juce::Slider& s, const juce::String& name)
{
    s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 56, 16);
    s.setName (name);
}
} // namespace

Rev2TrapAudioProcessorEditor::Rev2TrapAudioProcessorEditor (Rev2TrapAudioProcessor& p)
    : AudioProcessorEditor (p), processor_ (p)
{
    styleSlider (trapMacro_, "Trap");
    styleSlider (wtBlend_, "WT");
    styleSlider (cutoff_, "Cutoff");
    styleSlider (drive_, "Drive");
    addAndMakeVisible (trapMacro_);
    addAndMakeVisible (wtBlend_);
    addAndMakeVisible (cutoff_);
    addAndMakeVisible (drive_);
    presetLabel_.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (presetLabel_);

    auto& apvts = processor_.getAPVTS();
    trapAtt_ = std::make_unique<Att> (apvts, PID::trapMacro, trapMacro_);
    wtAtt_ = std::make_unique<Att> (apvts, PID::wavetableBlend, wtBlend_);
    cutoffAtt_ = std::make_unique<Att> (apvts, PID::cutoff, cutoff_);
    driveAtt_ = std::make_unique<Att> (apvts, PID::drive, drive_);

    setSize (520, 220);
}

void Rev2TrapAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff121018));
    g.setColour (juce::Colour (0xffc8b46a));
    g.setFont (16.0f);
    presetLabel_.setText (processor_.getProgramName (processor_.getCurrentProgram()), juce::dontSendNotification);
    g.drawText ("Rev2 Trap", getLocalBounds().removeFromTop (28), juce::Justification::centred);
}

void Rev2TrapAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (8);
    area.removeFromTop (24);
    presetLabel_.setBounds (area.removeFromTop (22));
    auto row = area.removeFromTop (120);
    const int w = row.getWidth() / 4;
    trapMacro_.setBounds (row.removeFromLeft (w).reduced (4));
    wtBlend_.setBounds (row.removeFromLeft (w).reduced (4));
    cutoff_.setBounds (row.removeFromLeft (w).reduced (4));
    drive_.setBounds (row.removeFromLeft (w).reduced (4));
}
