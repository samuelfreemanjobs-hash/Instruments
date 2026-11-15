#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "MpcSynthVoice.h"

class MpcAnalogSynthAudioProcessor : public juce::AudioProcessor {
public:
  static constexpr int kNumVoices = 6;
  static constexpr int kNumPrograms = 4;

  MpcAnalogSynthAudioProcessor();
  ~MpcAnalogSynthAudioProcessor() override = default;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override {}
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  juce::AudioProcessorEditor *createEditor() override { return nullptr; }
  bool hasEditor() const override { return false; }

  const juce::String getName() const override { return JucePlugin_Name; }
  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }
  double getTailLengthSeconds() const override { return 0.0; }

  int getNumPrograms() override { return kNumPrograms; }
  int getCurrentProgram() override { return currentProgram_; }
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int, const juce::String &) override {}

  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState &getAPVTS() { return apvts_; }

  static juce::AudioProcessorValueTreeState::ParameterLayout
  createParameterLayout();

private:
  void applyProgram(int index);

  juce::AudioProcessorValueTreeState apvts_;
  juce::Synthesiser synth_;
  int currentProgram_ = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MpcAnalogSynthAudioProcessor)
};