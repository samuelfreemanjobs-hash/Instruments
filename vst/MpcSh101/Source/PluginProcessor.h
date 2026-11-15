#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class MpcSh101AudioProcessor : public juce::AudioProcessor {
public:
  MpcSh101AudioProcessor();
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
  float processFilter(float);

  juce::AudioProcessorValueTreeState apvts_;
  juce::dsp::StateVariableTPTFilter<float> lpf_;
  juce::ADSR ampEnv_, filterEnv_;
  double sr_ = 44100.0;
  float phase_ = 0.0f, subPhase_ = 0.0f, pwmLfo_ = 0.0f;
  float freq_ = 110.0f, targetFreq_ = 110.0f, vel_ = 0.0f;
  int held_ = -1, prog_ = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MpcSh101AudioProcessor)
};