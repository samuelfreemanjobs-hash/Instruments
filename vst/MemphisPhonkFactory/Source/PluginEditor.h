#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

class MemphisPhonkFactoryAudioProcessorEditor
    : public juce::AudioProcessorEditor,
      private juce::AudioProcessorValueTreeState::Listener,
      private juce::Timer {
public:
  explicit MemphisPhonkFactoryAudioProcessorEditor(MemphisPhonkFactoryAudioProcessor &);
  ~MemphisPhonkFactoryAudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  MemphisPhonkFactoryAudioProcessor &processor_;

  juce::Slider bpmSlider_;
  juce::Slider swingSlider_;
  juce::Slider driveSlider_;
  juce::Slider hissSlider_;
  juce::Slider masterSlider_;
  juce::Slider variationSlider_;
  juce::ToggleButton previewButton_;
  juce::TextButton exportButton_{"Export MPC Kit"};
  juce::Label statusLabel_;

  using Att = juce::AudioProcessorValueTreeState::SliderAttachment;
  std::unique_ptr<Att> bpmAtt_, swingAtt_, driveAtt_, hissAtt_, masterAtt_, variationAtt_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> previewAtt_;

  void timerCallback() override;
  void exportKit();
  void parameterChanged(const juce::String &parameterID, float newValue) override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MemphisPhonkFactoryAudioProcessorEditor)
};
