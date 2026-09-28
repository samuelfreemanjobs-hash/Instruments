#include "PluginEditor.h"

#include "PluginProcessor.h"
#include "UI/Celestial/CelestialTheme.h"
#include "UI/UiLayout.h"

JunovaXAudioProcessorEditor::JunovaXAudioProcessorEditor (JunovaXAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor_ (p),
      celestialPanel_ (p),
      diagPanel_ (p)
{
    using Layout = junovax::ui::Layout;
    setSize (Layout::editorWidth, Layout::editorHeight);

    tabs_.addTab ("Main", junovax::ui::celestial::Theme::background(), &celestialPanel_, false);
    tabs_.addTab ("Diag", juce::Colour (0xff0f0f16), &diagPanel_, false);
    tabs_.setCurrentTabIndex (0);
    addAndMakeVisible (tabs_);
}

JunovaXAudioProcessorEditor::~JunovaXAudioProcessorEditor() = default;

void JunovaXAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff07070c));
}

void JunovaXAudioProcessorEditor::resized()
{
    tabs_.setBounds (getLocalBounds());
}
