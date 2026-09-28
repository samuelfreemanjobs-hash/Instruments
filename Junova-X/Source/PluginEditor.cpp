#include "PluginEditor.h"

#include "PluginProcessor.h"
#include "UI/UiLayout.h"

JunovaXAudioProcessorEditor::JunovaXAudioProcessorEditor (JunovaXAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor_ (p),
      mainPanel_ (p),
      diagPanel_ (p)
{
    using Layout = junovax::ui::Layout;
    setSize (Layout::editorWidth, Layout::editorHeight);

    tabs_.addTab ("Main", juce::Colour (0xff12121a), &mainPanel_, false);
    tabs_.addTab ("Diag", juce::Colour (0xff0f0f16), &diagPanel_, false);
    tabs_.setCurrentTabIndex (0);
    addAndMakeVisible (tabs_);
}

JunovaXAudioProcessorEditor::~JunovaXAudioProcessorEditor() = default;

void JunovaXAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0a0a10));
}

void JunovaXAudioProcessorEditor::resized()
{
    tabs_.setBounds (getLocalBounds());
}
