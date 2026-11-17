#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#if JUCE_WEB_BROWSER
 #include <juce_gui_extra/juce_gui_extra.h>
#endif

class PlaySeriesTemplateAudioProcessorEditor : public juce::AudioProcessorEditor,
                                               private juce::Timer {
public:
  explicit PlaySeriesTemplateAudioProcessorEditor(PlaySeriesTemplateAudioProcessor &);
  ~PlaySeriesTemplateAudioProcessorEditor() override;
  void resized() override;
  void paint(juce::Graphics &g) override;
  void timerCallback() override;

private:
  void applyUiScale();
  void loadWebHtml();

  PlaySeriesTemplateAudioProcessor &processor_;
  juce::TextButton diceButton_{"Dice"};
  juce::ToggleButton macroLock_{"Lock Macros"};
  juce::TextButton midiExport_{"Export MIDI"};
  juce::ComboBox uiScaleBox_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> macroLockAttachment_;
#if JUCE_WEB_BROWSER
  std::unique_ptr<juce::WebBrowserComponent> web_;
#endif
  std::array<std::unique_ptr<juce::Slider>, play_series::kMacroCount> macroSliders_;
  std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, play_series::kMacroCount>
      macroAttachments_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> uiScaleAttachment_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaySeriesTemplateAudioProcessorEditor)
};