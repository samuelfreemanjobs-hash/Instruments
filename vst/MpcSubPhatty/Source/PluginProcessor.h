#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class MpcSubPhattyAudioProcessor : public juce::AudioProcessor {
public:
  MpcSubPhattyAudioProcessor();
  ~MpcSubPhattyAudioProcessor() override = default;

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

  juce::AudioProcessorValueTreeState &getAPVTS() { return apvts_; }

  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
  void applyFactoryProgram(int index);
  void processLadderSample(float mixerIn, float &out);

  juce::AudioProcessorValueTreeState apvts_;
  juce::dsp::LadderFilter<float> ladderFilter_;
  juce::ADSR ampEnv_, filterEnv_;
  juce::Random noiseGen_;

  double sampleRate_ = 44100.0;
  float phase1_ = 0.0f, phase2_ = 0.0f, subPhase_ = 0.0f;
  float currentFreq_ = 55.0f, targetFreq_ = 55.0f;
  float noteVelocity_ = 0.0f;
  int heldNote_ = -1;
  int currentProgram_ = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MpcSubPhattyAudioProcessor)
};