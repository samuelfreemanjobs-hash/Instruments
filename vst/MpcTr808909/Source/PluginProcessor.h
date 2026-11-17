#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class MpcTr808909AudioProcessor : public juce::AudioProcessor {
public:
  MpcTr808909AudioProcessor();
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
  float renderKick();
  float renderSnare();
  float renderHat();

  juce::AudioProcessorValueTreeState apvts_;
  juce::Random rng_;
  double sr_ = 44100.0;
  int prog_ = 0;

  bool kickOn_ = false, snareOn_ = false, hatOn_ = false;
  float kickAmp_ = 0.0f, snareAmp_ = 0.0f, hatAmp_ = 0.0f;
  float kickPhase_ = 0.0f, snarePhase_ = 0.0f, hatPhase_ = 0.0f;
  float kickFreq_ = 110.0f, kickEnvT_ = 0.0f, kickVel_ = 1.0f;
  float snareTone_ = 180.0f, snareVel_ = 1.0f;
  float hatVel_ = 1.0f;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MpcTr808909AudioProcessor)
};