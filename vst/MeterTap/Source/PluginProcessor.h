#pragma once
/**
 * MeterTapAudioProcessor — DPCS Phase 2 contract header.
 *
 * Normative program chain: plugins/MeterTap/.ideas/architecture.md §2
 *   ScopedNoDenormals → sanitize → (bypass? passthrough : smoothed linear trim)
 *   → PeakRms analysis tap → MeterBridge.publish
 * Parameters: plugins/MeterTap/.ideas/parameter-spec.md v1
 *   dl_input_trim_db, dl_bypass, dl_peak_hold, dl_meter_floor_db
 */
#include <juce_audio_processors/juce_audio_processors.h>

#include "MeterBridge.h"
#include "PeakRms.h"
#include "parameter_smoothing.h"

class MeterTapAudioProcessor : public juce::AudioProcessor {
public:
  MeterTapAudioProcessor();
  ~MeterTapAudioProcessor() override;

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

  /** Host bypass shares the plugin parameter (architecture.md §2). */
  juce::AudioProcessorParameter* getBypassParameter() override;

  /** Lock-free bridge for the editor timer (UI pull). */
  dl::analysis::MeterBridge& meterBridge() noexcept { return bridge_; }
  const dl::analysis::MeterBridge& meterBridge() const noexcept { return bridge_; }

  /** Parameter IDs — parameter-spec v1. */
  static constexpr const char* kParamTrimDb = "dl_input_trim_db";
  static constexpr const char* kParamBypass = "dl_bypass";
  static constexpr const char* kParamPeakHold = "dl_peak_hold";
  static constexpr const char* kParamMeterFloorDb = "dl_meter_floor_db";

private:
  juce::AudioProcessorValueTreeState apvts_;
  double sampleRate_ = 48000.0;
  DspParamSmooth trimSmooth_;
  dl::analysis::PeakRms peakRms_;
  dl::analysis::MeterBridge bridge_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeterTapAudioProcessor)
};
