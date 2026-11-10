#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "FactoryPresets.h"
#include "StemShieldDsp.h"

class StemShieldAudioProcessor : public juce::AudioProcessor {
public:
  StemShieldAudioProcessor();
  ~StemShieldAudioProcessor() override;

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

  int getNumPrograms() override { return kStemShieldFactoryPresetCount; }
  int getCurrentProgram() override { return currentProgram_; }
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int, const juce::String&) override {}

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState& getAPVTS() { return apvts_; }
  const StemShieldMeterState& getMeters() const { return dsp_.getMeters(); }
  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  static constexpr const char* kParamCeiling = "ceiling";
  static constexpr const char* kParamSubBalance = "sub_balance";
  static constexpr const char* kParamSubCrossover = "sub_crossover";
  static constexpr const char* kParamRelease = "release";
  static constexpr const char* kParamLookahead = "lookahead";
  static constexpr const char* kParamGain = "gain";

private:
  void syncDspFromApvts();
  void applyFactoryPreset(int index);

  juce::AudioProcessorValueTreeState apvts_;
  StemShieldDsp dsp_;
  int currentProgram_ = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StemShieldAudioProcessor)
};
