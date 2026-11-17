#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class MpcTr808909AudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  explicit MpcTr808909AudioProcessorEditor(MpcTr808909AudioProcessor &);
  ~MpcTr808909AudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  MpcTr808909AudioProcessor &processor_;
  juce::Slider gainSlider_;
  juce::Slider waveSlider_;
  juce::Label gainLabel_;
  juce::Label waveLabel_;

  using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
  std::unique_ptr<SliderAttachment> gainAttachment_;
  std::unique_ptr<SliderAttachment> waveAttachment_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MpcTr808909AudioProcessorEditor)
};
