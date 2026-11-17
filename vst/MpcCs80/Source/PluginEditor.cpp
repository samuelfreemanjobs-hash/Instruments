#include "PluginEditor.h"

MpcCs80AudioProcessorEditor::MpcCs80AudioProcessorEditor(
    MpcCs80AudioProcessor &p)
    : AudioProcessorEditor(&p), processor_(p) {
  gainSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  waveSlider_.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  gainLabel_.setText("Gain", juce::dontSendNotification);
  waveLabel_.setText("Wave", juce::dontSendNotification);
  gainLabel_.attachToComponent(&gainSlider_, false);
  waveLabel_.attachToComponent(&waveSlider_, false);

  addAndMakeVisible(gainSlider_);
  addAndMakeVisible(waveSlider_);
  addAndMakeVisible(gainLabel_);
  addAndMakeVisible(waveLabel_);

  gainAttachment_ = std::make_unique<SliderAttachment>(processor_.getAPVTS(), "gain", gainSlider_);
  waveAttachment_ = std::make_unique<SliderAttachment>(processor_.getAPVTS(), "wave", waveSlider_);

  setSize(400, 240);
}

MpcCs80AudioProcessorEditor::~MpcCs80AudioProcessorEditor() = default;

void MpcCs80AudioProcessorEditor::paint(juce::Graphics &g) {
  g.fillAll(juce::Colour(0xff1a1a1e));
  g.setColour(juce::Colours::white);
  g.setFont(16.0f);
  g.drawFittedText("Template Synth (VSTi boilerplate)", getLocalBounds().removeFromTop(28),
                   juce::Justification::centred, 1);
}

void MpcCs80AudioProcessorEditor::resized() {
  auto area = getLocalBounds().reduced(12);
  area.removeFromTop(32);
  auto row = area.removeFromTop(120);
  gainSlider_.setBounds(row.removeFromLeft(row.getWidth() / 2).reduced(8));
  waveSlider_.setBounds(row.reduced(8));
}
