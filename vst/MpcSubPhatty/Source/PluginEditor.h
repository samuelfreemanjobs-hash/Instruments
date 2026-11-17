#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class MpcSubPhattyAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  explicit MpcSubPhattyAudioProcessorEditor(MpcSubPhattyAudioProcessor &);
  ~MpcSubPhattyAudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  MpcSubPhattyAudioProcessor &processor_;
  juce::Slider gainSlider_;
  juce::Slider waveSlider_;
  juce::Label gainLabel_;
  juce::Label waveLabel_;

  using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
  std::unique_ptr<SliderAttachment> gainAttachment_;
  std::unique_ptr<SliderAttachment> waveAttachment_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MpcSubPhattyAudioProcessorEditor)
};
