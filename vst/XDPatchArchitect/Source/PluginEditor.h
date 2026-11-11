#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "PluginProcessor.h"

class XDPatchArchitectAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  explicit XDPatchArchitectAudioProcessorEditor(XDPatchArchitectAudioProcessor &);
  ~XDPatchArchitectAudioProcessorEditor() override = default;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  XDPatchArchitectAudioProcessor &proc_;
  juce::Slider mutateSlider_;
  juce::TextButton sendButton_{"Send to Minilogue XD"};
  juce::TextButton bassButton_{"Bass"};
  juce::TextButton padButton_{"Pad"};
  juce::TextButton leadButton_{"Lead"};

  void applyProfile(float cutoff, float reso, float attack, float release);
  void wireSlider(juce::Slider &slider, const juce::String &paramId);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XDPatchArchitectAudioProcessorEditor)
};
