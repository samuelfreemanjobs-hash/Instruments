#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "PluginProcessor.h"

class DrumCrushAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     private juce::ComboBox::Listener {
public:
  explicit DrumCrushAudioProcessorEditor(DrumCrushAudioProcessor&);
  ~DrumCrushAudioProcessorEditor() override;

  void paint(juce::Graphics& g) override;
  void resized() override;

private:
  void comboBoxChanged(juce::ComboBox* box) override;

  class KnobLookAndFeel : public juce::LookAndFeel_V4 {
  public:
    KnobLookAndFeel();
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;
  };

  DrumCrushAudioProcessor& proc_;
  KnobLookAndFeel laf_;

  juce::Label titleLabel_;
  juce::Label subtitleLabel_;
  juce::ComboBox presetBox_;

  juce::Slider punchKnob_;
  juce::Slider crushKnob_;
  juce::Slider clipKnob_;
  juce::Slider mixKnob_;
  juce::Slider outputKnob_;

  juce::Label punchLabel_;
  juce::Label crushLabel_;
  juce::Label clipLabel_;
  juce::Label mixLabel_;
  juce::Label outputLabel_;

  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> punchAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crushAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> clipAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttach_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumCrushAudioProcessorEditor)
};
