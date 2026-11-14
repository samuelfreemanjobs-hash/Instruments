#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "PluginProcessor.h"

class StemShieldAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       private juce::ComboBox::Listener,
                                       private juce::Timer {
public:
  explicit StemShieldAudioProcessorEditor(StemShieldAudioProcessor&);
  ~StemShieldAudioProcessorEditor() override;

  void paint(juce::Graphics& g) override;
  void resized() override;

private:
  void comboBoxChanged(juce::ComboBox* box) override;
  void timerCallback() override;

  class KnobLookAndFeel : public juce::LookAndFeel_V4 {
  public:
    KnobLookAndFeel();
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;
  };

  void paintMeters(juce::Graphics& g);

  StemShieldAudioProcessor& proc_;
  KnobLookAndFeel laf_;

  juce::Label titleLabel_;
  juce::Label subtitleLabel_;
  juce::ComboBox presetBox_;

  juce::Slider ceilingKnob_;
  juce::Slider subBalanceKnob_;
  juce::Slider crossoverKnob_;
  juce::Slider releaseKnob_;
  juce::Slider lookaheadKnob_;
  juce::Slider gainKnob_;

  juce::Label ceilingLabel_;
  juce::Label subBalanceLabel_;
  juce::Label crossoverLabel_;
  juce::Label releaseLabel_;
  juce::Label lookaheadLabel_;
  juce::Label gainLabel_;

  juce::Rectangle<int> outputMeterRect_;
  juce::Rectangle<int> grMeterRect_;

  float meterOutDb_ = -60.0f;
  float meterGrDb_ = 0.0f;

  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ceilingAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> subBalanceAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crossoverAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lookaheadAttach_;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttach_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StemShieldAudioProcessorEditor)
};
