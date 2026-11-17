#pragma once

#include <array>

#include "FactoryPrograms.h"
#include "JunoVoice.h"
#include <juce_audio_processors/juce_audio_processors.h>

class MpcJuno106AudioProcessor : public juce::AudioProcessor {
public:
  MpcJuno106AudioProcessor();
  void prepareToPlay(double, int) override;
  void releaseResources() override {}
  bool isBusesLayoutSupported(const BusesLayout &) const override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
  juce::AudioProcessorEditor *createEditor() override { return nullptr; }
  bool hasEditor() const override { return false; }
  const juce::String getName() const override { return JucePlugin_Name; }
  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return false; }
  double getTailLengthSeconds() const override { return 1.5; }
  int getNumPrograms() override { return juno106::kNumPrograms; }
  int getCurrentProgram() override { return prog_; }
  void setCurrentProgram(int) override;
  const juce::String getProgramName(int) override;
  void changeProgramName(int, const juce::String &) override {}
  void getStateInformation(juce::MemoryBlock &) override;
  void setStateInformation(const void *, int) override;
  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
  void applyProgram(int);
  void processChorus(juce::AudioBuffer<float> &buf, float mix);

  juce::AudioProcessorValueTreeState apvts_;
  juce::Synthesiser synth_;
  JunoSound sound_;
  int prog_ = 0;
  static constexpr int kVoices = 6;
  double sr_ = 44100.0;
  float chorusPhase_ = 0.0f;
  std::array<float, 8192> delayL_{};
  std::array<float, 8192> delayR_{};
  int writePos_ = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MpcJuno106AudioProcessor)
};