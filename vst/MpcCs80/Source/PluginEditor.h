#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class MpcCs80AudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  explicit MpcCs80AudioProcessorEditor(MpcCs80AudioProcessor &);
  ~MpcCs80AudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  MpcCs80AudioProcessor &processor_;
  juce::Slider gainSlider_;
  juce::Slider waveSlider_;
  juce::Label gainLabel_;
  juce::Label waveLabel_;

  using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
  std::unique_ptr<SliderAttachment> gainAttachment_;
  std::unique_ptr<SliderAttachment> waveAttachment_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MpcCs80AudioProcessorEditor)
};
