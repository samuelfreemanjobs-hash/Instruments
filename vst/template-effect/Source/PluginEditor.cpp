#include "PluginEditor.h"

TemplateEffectAudioProcessorEditor::TemplateEffectAudioProcessorEditor(
    TemplateEffectAudioProcessor &p)
    : AudioProcessorEditor(&p), processor_(p) {
  for (auto *s : {&driveSlider_, &toneSlider_, &mixSlider_}) {
    s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(s);
  }
  driveLabel_.setText("Drive", juce::dontSendNotification);
  toneLabel_.setText("Tone", juce::dontSendNotification);
  mixLabel_.setText("Mix", juce::dontSendNotification);
  for (auto *l : {&driveLabel_, &toneLabel_, &mixLabel_}) {
    addAndMakeVisible(l);
  }

  driveAttachment_ =
      std::make_unique<SliderAttachment>(processor_.getAPVTS(), "drive", driveSlider_);
  toneAttachment_ = std::make_unique<SliderAttachment>(processor_.getAPVTS(), "tone", toneSlider_);
  mixAttachment_ = std::make_unique<SliderAttachment>(processor_.getAPVTS(), "mix", mixSlider_);

  setSize(420, 260);
}

TemplateEffectAudioProcessorEditor::~TemplateEffectAudioProcessorEditor() = default;

void TemplateEffectAudioProcessorEditor::paint(juce::Graphics &g) {
  g.fillAll(juce::Colour(0xff141820));
  g.setColour(juce::Colours::white);
  g.setFont(16.0f);
  g.drawFittedText("Template Effect (drive + tone LP + dry/wet)", getLocalBounds().removeFromTop(28),
                   juce::Justification::centred, 1);
}

void TemplateEffectAudioProcessorEditor::resized() {
  auto area = getLocalBounds().reduced(12);
  area.removeFromTop(32);
  auto row = area.removeFromTop(140);
  const int w = row.getWidth() / 3;
  driveSlider_.setBounds(row.removeFromLeft(w).reduced(6));
  toneSlider_.setBounds(row.removeFromLeft(w).reduced(6));
  mixSlider_.setBounds(row.reduced(6));
  driveLabel_.setBounds(driveSlider_.getX(), driveSlider_.getBottom() + 2, driveSlider_.getWidth(),
                        20);
  toneLabel_.setBounds(toneSlider_.getX(), toneSlider_.getBottom() + 2, toneSlider_.getWidth(), 20);
  mixLabel_.setBounds(mixSlider_.getX(), mixSlider_.getBottom() + 2, mixSlider_.getWidth(), 20);
}
