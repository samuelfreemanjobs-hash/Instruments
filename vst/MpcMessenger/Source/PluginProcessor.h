#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class MpcMessengerAudioProcessor : public juce::AudioProcessor {
public:
  MpcMessengerAudioProcessor();
  ~MpcMessengerAudioProcessor() override = default;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override {}
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  juce::AudioProcessorEditor *createEditor() override { return nullptr; }
  bool hasEditor() const override { return false; }

  const juce::String getName() const override { return JucePlugin_Name; }
  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return false; }
  double getTailLengthSeconds() const override { return 0.0; }

  int getNumPrograms() override;
  int getCurrentProgram() override { return currentProgram_; }
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int, const juce::String &) override {}

  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
  juce::AudioProcessorValueTreeState &getAPVTS() { return apvts_; }

private:
  void applyFactoryProgram(int index);
  float processLadder(float in);

  juce::AudioProcessorValueTreeState apvts_;
  juce::dsp::LadderFilter<float> ladder_;
  juce::ADSR ampEnv_, filterEnv_;
  juce::Random noiseGen_;

  double sampleRate_ = 44100.0;
  float phase1_ = 0.0f, phase2_ = 0.0f, subPhase_ = 0.0f;
  float currentFreq_ = 110.0f, targetFreq_ = 110.0f;
  float noteVelocity_ = 0.0f;
  int heldNote_ = -1;
  int currentProgram_ = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MpcMessengerAudioProcessor)
};