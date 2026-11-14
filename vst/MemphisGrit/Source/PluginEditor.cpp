#include "PluginEditor.h"

MemphisGritAudioProcessorEditor::MemphisGritAudioProcessorEditor(MemphisGritAudioProcessor& p)
    : AudioProcessorEditor(&p), processor_(p) {
  for (auto* s : {&driveSlider_, &lofiSlider_, &midSlider_, &mixSlider_, &outputSlider_}) {
    s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(s);
  }
  driveLabel_.setText("Drive", juce::dontSendNotification);
  lofiLabel_.setText("Lo-Fi", juce::dontSendNotification);
  midLabel_.setText("Mid Grit", juce::dontSendNotification);
  mixLabel_.setText("Mix", juce::dontSendNotification);
  outputLabel_.setText("Output", juce::dontSendNotification);
  for (auto* l : {&driveLabel_, &lofiLabel_, &midLabel_, &mixLabel_, &outputLabel_}) {
    addAndMakeVisible(l);
  }
  bypassButton_.setButtonText("Bypass");
  addAndMakeVisible(bypassButton_);

  driveAttachment_ = std::make_unique<SliderAttachment>(
      processor_.getAPVTS(), MemphisGritAudioProcessor::kParamDrive, driveSlider_);
  lofiAttachment_ = std::make_unique<SliderAttachment>(
      processor_.getAPVTS(), MemphisGritAudioProcessor::kParamLofi, lofiSlider_);
  midAttachment_ = std::make_unique<SliderAttachment>(processor_.getAPVTS(),
                                                      MemphisGritAudioProcessor::kParamMid,
                                                      midSlider_);
  mixAttachment_ = std::make_unique<SliderAttachment>(processor_.getAPVTS(),
                                                      MemphisGritAudioProcessor::kParamMix,
                                                      mixSlider_);
  outputAttachment_ = std::make_unique<SliderAttachment>(
      processor_.getAPVTS(), MemphisGritAudioProcessor::kParamOutputDb, outputSlider_);
  bypassAttachment_ = std::make_unique<ButtonAttachment>(
      processor_.getAPVTS(), MemphisGritAudioProcessor::kParamBypass, bypassButton_);

  setSize(560, 300);
}

MemphisGritAudioProcessorEditor::~MemphisGritAudioProcessorEditor() = default;

void MemphisGritAudioProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(juce::Colour(0xff121018));
  g.setColour(juce::Colour(0xffc9a0ff));
  g.setFont(juce::Font(18.0f, juce::Font::bold));
  g.drawFittedText("MEMPHIS GRIT", getLocalBounds().removeFromTop(28), juce::Justification::centred,
                   1);
  g.setColour(juce::Colours::grey);
  g.setFont(12.0f);
  g.drawFittedText("DiskLordz FX — lo-fi distortion & sampler degrade", getLocalBounds().removeFromTop(48),
                   juce::Justification::centred, 1);
}

void MemphisGritAudioProcessorEditor::resized() {
  auto area = getLocalBounds().reduced(12);
  area.removeFromTop(52);
  bypassButton_.setBounds(area.removeFromTop(28).withWidth(100));
  auto row = area.removeFromTop(160);
  const int w = row.getWidth() / 5;
  auto place = [&](juce::Slider& s, juce::Label& l) {
    auto cell = row.removeFromLeft(w).reduced(4);
    s.setBounds(cell.removeFromTop(cell.getHeight() - 22));
    l.setBounds(cell);
  };
  place(driveSlider_, driveLabel_);
  place(lofiSlider_, lofiLabel_);
  place(midSlider_, midLabel_);
  place(mixSlider_, mixLabel_);
  place(outputSlider_, outputLabel_);
}
