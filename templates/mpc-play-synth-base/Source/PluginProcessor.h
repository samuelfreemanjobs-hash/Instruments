#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "FactoryPrograms.h"
#include "MacroRouter.h"
#include "PlaySeriesCommon.h"
#include "PlaySynthSound.h"
#include "PlaySynthVoice.h"

class MpcPlayTemplateAudioProcessor : public juce::AudioProcessor {
public:
  MpcPlayTemplateAudioProcessor();
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
  int getNumPrograms() override { return mpc_play::kProgramCount; }
  int getCurrentProgram() override { return currentProgram_; }
  void setCurrentProgram(int) override;
  const juce::String getProgramName(int) override;
  void changeProgramName(int, const juce::String &) override {}
  void getStateInformation(juce::MemoryBlock &) override;
  void setStateInformation(const void *, int) override;

  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
  play_series::MacroTargets readTargets() const noexcept;

  juce::AudioProcessorValueTreeState apvts_;
  juce::Synthesiser synth_;
  PlaySynthSound sound_;
  play_series::MacroRouter router_;
  int currentProgram_ = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MpcPlayTemplateAudioProcessor)
};