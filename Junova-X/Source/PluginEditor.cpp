#include "PluginEditor.h"

#include "PluginProcessor.h"
#include "UI/Celestial/CelestialTheme.h"
#include "UI/UiLayout.h"

JunovaXAudioProcessorEditor::JunovaXAudioProcessorEditor (JunovaXAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor_ (p),
      celestialPanel_ (p),
      vault_ (p, [this] { vault_.hideOverlay(); }),
      diagPanel_ (p)
{
    setLookAndFeel (&skin_);
    setSize (junovax::ui::Layout::editorWidth, junovax::ui::Layout::editorHeight);

    celestialPanel_.setVaultOpener ([this] { vault_.showOverlay(); });
    addAndMakeVisible (celestialPanel_);
    addChildComponent (vault_);
    addChildComponent (diagPanel_);
}

JunovaXAudioProcessorEditor::~JunovaXAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void JunovaXAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (junovax::ui::celestial::Theme::background());
}

void JunovaXAudioProcessorEditor::resized()
{
    celestialPanel_.setBounds (getLocalBounds());
    vault_.setBounds (getLocalBounds());
}

void JunovaXAudioProcessorEditor::syncForSnapshot()
{
    celestialPanel_.syncPerformanceMeters();
}