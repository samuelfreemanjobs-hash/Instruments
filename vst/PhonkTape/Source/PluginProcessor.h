#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "FactoryPresets.h"
#include "PhonkTapeDsp.h"

class PhonkTapeAudioProcessor : public juce::AudioProcessor {
public:
  PhonkTapeAudioProcessor();
  ~PhonkTapeAudioProcessor() override;

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

  static constexpr const char* kParamDrive = "drive";
  static constexpr const char* kParamWow = "wow";
  static constexpr const char* kParamFlutter = "flutter";
  static constexpr const char* kParamAge = "age";
  static constexpr const char* kParamNoise = "noise";
  static constexpr const char* kParamMix = "mix";
  static constexpr const char* kParamOutputGain = "output_gain";

private:
  void applyFactoryPreset(int index);

  juce::AudioProcessorValueTreeState apvts_;
  PhonkTapeDsp dsp_;
  int currentProgram_ = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhonkTapeAudioProcessor)
};
