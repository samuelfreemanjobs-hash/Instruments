#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "FactoryPresets.h"
#include "PunisherDsp.h"

class Punisher808AudioProcessor : public juce::AudioProcessor {
public:
  Punisher808AudioProcessor();
  ~Punisher808AudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override { return true; }

  const juce::String getName() const override { return JucePlugin_Name; }
  bool acceptsMidi() const override { return false; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }
  double getTailLengthSeconds() const override { return 0.0; }

  int getNumPrograms() override { return kFactoryPresetCount; }
  int getCurrentProgram() override { return currentProgram_; }
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int, const juce::String&) override {}

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState& getAPVTS() { return apvts_; }
  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  static constexpr const char* kParamPunish = "punish";
  static constexpr const char* kParamHarmonicShape = "harmonic_shape";
  static constexpr const char* kParamSubCrossover = "sub_crossover";
  static constexpr const char* kParamMix = "mix";
  static constexpr const char* kParamOutputGain = "output_gain";

private:
  void applyFactoryPreset(int index);

  juce::AudioProcessorValueTreeState apvts_;
  PunisherDsp dsp_;
  int currentProgram_ = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Punisher808AudioProcessor)
};
