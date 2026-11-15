#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "PlaySeriesCommon.h"

// Rename class + file when cloning for AnalogDreams84 / CloudSupplyTrap / LoFiGlowTape.
class PlaySeriesTemplateAudioProcessor : public juce::AudioProcessor {
public:
  PlaySeriesTemplateAudioProcessor();
  void prepareToPlay(double, int) override;
  void releaseResources() override {}
  bool isBusesLayoutSupported(const BusesLayout &) const override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override { return true; }
  const juce::String getName() const override { return JucePlugin_Name; }
  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return false; }
  double getTailLengthSeconds() const override { return 0.0; }
  int getNumPrograms() override { return play_series::kFactoryPresetCount; }
  int getCurrentProgram() override { return currentProgram_; }
  void setCurrentProgram(int) override;
  const juce::String getProgramName(int) override;
  void changeProgramName(int, const juce::String &) override {}
  void getStateInformation(juce::MemoryBlock &) override;
  void setStateInformation(const void *, int) override;

  juce::AudioProcessorValueTreeState &getAPVTS() noexcept { return apvts_; }

  /** Call from UI/message thread only (Dice button). */
  void randomizePatchBounded();

  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
  void applyFactoryPreset(int index);
  float effectiveBlend() const noexcept;

  juce::AudioProcessorValueTreeState apvts_;
  juce::Random rng_;
  int currentProgram_ = 0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaySeriesTemplateAudioProcessor)
};