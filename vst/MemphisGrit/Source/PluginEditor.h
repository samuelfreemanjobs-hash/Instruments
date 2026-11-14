#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class MemphisGritAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  explicit MemphisGritAudioProcessorEditor(MemphisGritAudioProcessor&);
  ~MemphisGritAudioProcessorEditor() override;

  void paint(juce::Graphics&) override;
  void resized() override;

private:
  MemphisGritAudioProcessor& processor_;
  juce::Slider driveSlider_;
  juce::Slider lofiSlider_;
  juce::Slider midSlider_;
  juce::Slider mixSlider_;
  juce::Slider outputSlider_;
  juce::ToggleButton bypassButton_;
  juce::Label driveLabel_;
  juce::Label lofiLabel_;
  juce::Label midLabel_;
  juce::Label mixLabel_;
  juce::Label outputLabel_;

  using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
  using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
  std::unique_ptr<SliderAttachment> driveAttachment_;
  std::unique_ptr<SliderAttachment> lofiAttachment_;
  std::unique_ptr<SliderAttachment> midAttachment_;
  std::unique_ptr<SliderAttachment> mixAttachment_;
  std::unique_ptr<SliderAttachment> outputAttachment_;
  std::unique_ptr<ButtonAttachment> bypassAttachment_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MemphisGritAudioProcessorEditor)
};
