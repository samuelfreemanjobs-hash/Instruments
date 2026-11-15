#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "MacroRouter.h"
#include "PerformanceEngine.h"
#include "PlaySeriesCommon.h"
#include "PlaySynthSound.h"
#include "PlaySynthVoice.h"
#include "ScopeRingBuffer.h"

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

  void randomizePatchBounded();
  int readScopePeaks(float *dest, int maxItems) const noexcept;
  void exportMidiPattern();

  static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
  void applyFactoryPreset(int index);
  float effectiveBlend() const noexcept;
  play_series::MacroTargets readMacroTargets() const noexcept;

  juce::AudioProcessorValueTreeState apvts_;
  juce::Synthesiser synth_;
  PlaySynthSound sound_;
  play_series::PerformanceEngine performance_;
  play_series::MacroRouter macroRouter_;
  juce::dsp::Oversampling<float> masterOversample_{2, 1,
                                                   juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR};
  juce::SmoothedValue<float> spaceSend_, delaySend_;
  mutable play_series::ScopeRingBuffer scope_;
  juce::Random rng_;
  int currentProgram_ = 0;
  double currentBpm_ = 120.0;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaySeriesTemplateAudioProcessor)
};