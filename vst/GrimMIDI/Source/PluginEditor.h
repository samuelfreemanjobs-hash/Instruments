#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "PluginProcessor.h"

class GrimMIDIAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     private juce::ComboBox::Listener,
                                     private juce::Button::Listener {
public:
  explicit GrimMIDIAudioProcessorEditor(GrimMIDIAudioProcessor&);
  ~GrimMIDIAudioProcessorEditor() override;

  void paint(juce::Graphics& g) override;
  void resized() override;

private:
  void comboBoxChanged(juce::ComboBox* box) override;
  void buttonClicked(juce::Button* button) override;
  juce::File writeProgressionToTempMidi() const;

  GrimMIDIAudioProcessor& proc_;

  juce::Label titleLabel_;
  juce::Label subtitleLabel_;
  juce::ComboBox presetBox_;

  juce::Slider spreadSlider_;
  juce::Slider strumSlider_;
  juce::Slider velSlider_;
  juce::Slider octaveSlider_;

  juce::ComboBox keyBox_;
  juce::ComboBox scaleBox_;
  juce::ComboBox chordBox_;
  juce::ComboBox inversionBox_;

  juce::Label spreadLabel_;
  juce::Label strumLabel_;
  juce::Label velLabel_;
  juce::Label octaveLabel_;

  juce::TextButton exportDragButton_{"Drag MIDI to DAW"};

  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> spreadAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> strumAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> velAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> octaveAttach_;

  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> scaleAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> chordAttach_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrimMIDIAudioProcessorEditor)
};
