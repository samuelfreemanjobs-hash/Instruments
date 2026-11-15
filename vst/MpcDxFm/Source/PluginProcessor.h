#pragma once

#include <array>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Dsp/FmVoice.h"

class MpcDxFmAudioProcessor : public juce::AudioProcessor {
public:
  static constexpr int kMaxVoices = 12;

  MpcDxFmAudioProcessor();
  ~MpcDxFmAudioProcessor() override = default;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override {}
  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  juce::AudioProcessorEditor *createEditor() override { return nullptr; }
  bool hasEditor() const override { return false; }

  const juce::String getName() const override { return JucePlugin_Name; }
  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return false; }
  double getTailLengthSeconds() const override { return 0.3; }

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
  void syncVoiceParams(dxfm::VoiceParams &vp) const;
  int findFreeVoice();
  int findVoiceForNote(int note);

  juce::AudioProcessorValueTreeState apvts_;
  std::array<dxfm::FmVoice, kMaxVoices> voices_{};
  double sampleRate_ = 44100.0;
  float lfoPhase_ = 0.0f;
  int currentProgram_ = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MpcDxFmAudioProcessor)
};