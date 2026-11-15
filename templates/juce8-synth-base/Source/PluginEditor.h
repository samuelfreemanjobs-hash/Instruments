#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include "PluginProcessor.h"

class PlaySeriesTemplateAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
  explicit PlaySeriesTemplateAudioProcessorEditor(PlaySeriesTemplateAudioProcessor &);
  void resized() override;

private:
  PlaySeriesTemplateAudioProcessor &processor_;
  juce::TextButton diceButton_{"Dice"};
  std::unique_ptr<juce::WebBrowserComponent> web_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaySeriesTemplateAudioProcessorEditor)
};