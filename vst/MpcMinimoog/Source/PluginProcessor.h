#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class MpcMinimoogAudioProcessor : public juce::AudioProcessor {
public:
  MpcMinimoogAudioProcessor();
  void prepareToPlay(double, int) override;
  void releaseResources() override {}
  bool isBusesLayoutSupported(const BusesLayout &) const override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
  juce::AudioProcessorEditor *createEditor() override { return nullptr; }
  bool hasEditor() const override { return false; }
  const juce::String getName() const override { return JucePlugin_Name; }
  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return false; }
  double getTailLengthSeconds() const override { return 0.0; }
  int getNumPrograms() override;
  int getCurrentProgram() override { return prog_; }
  void setCurrentProgram(int) override;
  const juce::String getProgramName(int) override;
  void changeProgramName(int, const juce::String &) override {}
  void getStateInformation(juce::MemoryBlock &) override;
  void setStateInformation(const void *, int) override;
  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
  void applyProgram(int);
  float ladderOne(float);

  juce::AudioProcessorValueTreeState apvts_;
  juce::dsp::LadderFilter<float> ladder_;
  juce::ADSR ampEnv_, filterEnv_;
  juce::Random noise_;
  double sr_ = 44100.0;
  float p1_ = 0.0f, p2_ = 0.0f, p3_ = 0.0f;
  float freq_ = 110.0f, target_ = 110.0f, vel_ = 0.0f;
  int held_ = -1, prog_ = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MpcMinimoogAudioProcessor)
};