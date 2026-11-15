#include "PluginEditor.h"

#include "PluginProcessor.h"
#include "UI/Celestial/CelestialTheme.h"

JunovaXAudioProcessorEditor::JunovaXAudioProcessorEditor (JunovaXAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor_ (p),
      celestialPanel_ (p),
      vault_ (p, [this] { vault_.hideOverlay(); }),
      diagPanel_ (p)
{
    using Theme = junovax::ui::celestial::Theme;

    setLookAndFeel (&skin_);
    using Layout = junovax::ui::Layout;
    setSize (Layout::editorWidth, Layout::editorHeight + 72);

    celestialPanel_.setVaultOpener ([this] { vault_.showOverlay(); });
    addAndMakeVisible (celestialPanel_);
    addChildComponent (vault_);
    addChildComponent (diagPanel_);

    keyboard_ = std::make_unique<juce::MidiKeyboardComponent> (
        processor_.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard);
    keyboard_->setAvailableRange (36, 96);
    keyboard_->setKeyWidth (18.0f);
    addAndMakeVisible (*keyboard_);

    pitchWheel_.setRange (0.0, 1.0);
    pitchWheel_.setSliderStyle (juce::Slider::LinearVertical);
    pitchWheel_.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    modWheel_.setRange (0.0, 1.0);
    modWheel_.setSliderStyle (juce::Slider::LinearVertical);
    modWheel_.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible (pitchWheel_);
    addAndMakeVisible (modWheel_);

    for (auto* b : { &transposeDown_, &transposeZero_, &transposeUp_ })
        addAndMakeVisible (*b);
    transposeDown_.onClick = [this] { transposeOctaves_ = juce::jmax (-2, transposeOctaves_ - 1); };
    transposeZero_.onClick = [this] { transposeOctaves_ = 0; };
    transposeUp_.onClick = [this] { transposeOctaves_ = juce::jmin (2, transposeOctaves_ + 1); };

    applyZoom();
    startTimerHz (30);
}

JunovaXAudioProcessorEditor::~JunovaXAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void JunovaXAudioProcessorEditor::applyZoom()
{
    if (auto* zoom = processor_.getApvts().getRawParameterValue (junovax::ParameterIDs::editorZoom))
    {
        const float z = zoom->load();
        const int baseW = junovax::ui::Layout::editorWidth;
        const int baseH = junovax::ui::Layout::editorHeight + 72;
        setSize (static_cast<int> (baseW * z), static_cast<int> (baseH * z));
    }
}

void JunovaXAudioProcessorEditor::timerCallback()
{
    applyZoom();
}

void JunovaXAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (junovax::ui::celestial::Theme::background());
}

void JunovaXAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    const int kbH = 68;
    auto kbArea = area.removeFromBottom (kbH);
    celestialPanel_.setBounds (area);

    const int wheelW = 28;
    pitchWheel_.setBounds (kbArea.removeFromLeft (wheelW).reduced (4));
    modWheel_.setBounds (kbArea.removeFromLeft (wheelW).reduced (4));
    auto tr = kbArea.removeFromRight (120);
    transposeDown_.setBounds (tr.removeFromTop (20).reduced (2));
    transposeZero_.setBounds (tr.removeFromTop (20).reduced (2));
    transposeUp_.setBounds (tr.removeFromTop (20).reduced (2));
    if (keyboard_ != nullptr)
        keyboard_->setBounds (kbArea);

    vault_.setBounds (getLocalBounds());
}