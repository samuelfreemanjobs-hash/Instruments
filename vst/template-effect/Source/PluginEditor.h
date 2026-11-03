#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class TemplateEffectAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  explicit TemplateEffectAudioProcessorEditor(TemplateEffectAudioProcessor &);
  ~TemplateEffectAudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  TemplateEffectAudioProcessor &processor_;
  juce::Slider driveSlider_;
  juce::Slider toneSlider_;
  juce::Slider mixSlider_;
  juce::Label driveLabel_;
  juce::Label toneLabel_;
  juce::Label mixLabel_;

  using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
  std::unique_ptr<SliderAttachment> driveAttachment_;
  std::unique_ptr<SliderAttachment> toneAttachment_;
  std::unique_ptr<SliderAttachment> mixAttachment_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TemplateEffectAudioProcessorEditor)
};
