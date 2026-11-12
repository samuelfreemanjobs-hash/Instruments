#pragma once

#include "MemphisGritDsp.h"
#include <juce_audio_processors/juce_audio_processors.h>

class MemphisGritAudioProcessor : public juce::AudioProcessor {
public:
  MemphisGritAudioProcessor();
  ~MemphisGritAudioProcessor() override;

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

  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int) override {}
  const juce::String getProgramName(int) override { return "Init"; }
  void changeProgramName(int, const juce::String&) override {}

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState& getAPVTS() { return apvts_; }
  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  juce::AudioProcessorParameter* getBypassParameter() const override;

  static constexpr const char* kParamDrive = "mg_drive";
  static constexpr const char* kParamLofi = "mg_lofi";
  static constexpr const char* kParamMid = "mg_mid";
  static constexpr const char* kParamMix = "mg_mix";
  static constexpr const char* kParamOutputDb = "mg_output_db";
  static constexpr const char* kParamBypass = "mg_bypass";

private:
  void syncDspFromParameters();

  juce::AudioProcessorValueTreeState apvts_;
  MemphisGritDsp dsp_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MemphisGritAudioProcessor)
};
