#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "PluginProcessor.h"

class Punisher808AudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::ComboBox::Listener {
public:
  explicit Punisher808AudioProcessorEditor(Punisher808AudioProcessor&);
  ~Punisher808AudioProcessorEditor() override;

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

  Punisher808AudioProcessor& proc_;
  KnobLookAndFeel laf_;

  juce::Label titleLabel_;
  juce::Label subtitleLabel_;
  juce::ComboBox presetBox_;

  juce::Slider punishKnob_;
  juce::Slider shapeKnob_;
  juce::Slider crossoverKnob_;
  juce::Slider mixKnob_;
  juce::Slider outputKnob_;

  juce::Label punishLabel_;
  juce::Label shapeLabel_;
  juce::Label crossoverLabel_;
  juce::Label mixLabel_;
  juce::Label outputLabel_;

  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> punishAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> shapeAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crossoverAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttach_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Punisher808AudioProcessorEditor)
};
