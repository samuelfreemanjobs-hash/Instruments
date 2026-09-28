#include "DiagPanel.h"

#include "PluginProcessor.h"

namespace junovax::ui
{
DiagPanel::DiagPanel (JunovaXAudioProcessor& processor)
    : processor_ (processor)
{
    setLookAndFeel (&laf_);

    title_.setFont (juce::Font (22.0f, juce::Font::bold));
    addAndMakeVisible (title_);

    testToneButton_.setClickingTogglesState (true);
    addAndMakeVisible (testToneButton_);
    toneAttachment_ = std::make_unique<ButtonAttachment> (processor_.getApvts(), ParameterIDs::diagTestTone, testToneButton_);

    freqSlider_.setSliderStyle (juce::Slider::LinearHorizontal);
    freqSlider_.setTextBoxStyle (juce::Slider::TextBoxRight, false, 72, 22);
    addAndMakeVisible (freqSlider_);
    freqAttachment_ = std::make_unique<SliderAttachment> (processor_.getApvts(), ParameterIDs::diagToneFreq, freqSlider_);

    panicButton_.onClick = [this] { processor_.panicAllNotes(); };
    addAndMakeVisible (panicButton_);
}

DiagPanel::~DiagPanel()
{
    setLookAndFeel (nullptr);
}

void DiagPanel::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0f0f16));
    g.setColour (juce::Colour (0xff353548));
    g.drawRoundedRectangle (16.0f, 56.0f, static_cast<float> (getWidth()) - 32.0f, 120.0f, 8.0f, 1.5f);
}

void DiagPanel::resized()
{
    title_.setBounds (Layout::margin, Layout::margin, 240, 28);
    testToneButton_.setBounds (Layout::margin, 72, 160, 32);
    freqSlider_.setBounds (Layout::margin, 112, getWidth() - Layout::margin * 2, 32);
    panicButton_.setBounds (Layout::margin, 160, 220, 36);
}
} // namespace junovax::ui
