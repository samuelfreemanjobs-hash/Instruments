#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "FactoryPresets.h"
#include "PluginProcessor.h"

class PhonkTapeAudioProcessorEditor : public juce::AudioProcessorEditor,
                                      private juce::ComboBox::Listener {
public:
  explicit PhonkTapeAudioProcessorEditor(PhonkTapeAudioProcessor&);
  ~PhonkTapeAudioProcessorEditor() override;

  void paint(juce::Graphics&) override;
  void resized() override;

private:
  void comboBoxChanged(juce::ComboBox* box) override;

  class KnobLookAndFeel : public juce::LookAndFeel_V4 {
  public:
    KnobLookAndFeel();
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;
  };

  PhonkTapeAudioProcessor& proc_;
  KnobLookAndFeel laf_;

  juce::Label titleLabel_;
  juce::Label subtitleLabel_;
  juce::ComboBox presetBox_;

  juce::Slider driveKnob_, wowKnob_, flutterKnob_, ageKnob_, noiseKnob_, mixKnob_, outputKnob_;
  juce::Label driveLabel_, wowLabel_, flutterLabel_, ageLabel_, noiseLabel_, mixLabel_, outputLabel_;

  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> wowAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flutterAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ageAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noiseAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttach_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhonkTapeAudioProcessorEditor)
};
