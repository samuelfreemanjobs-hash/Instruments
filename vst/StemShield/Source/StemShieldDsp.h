#pragma once

/**
 * DiskLordz Stem-Shield program chain — stem export peak containment + sub balance.
 *
 * Path:
 *   Input gain -> Linkwitz-Riley sub split -> dynamic sub-band gain (SUB BALANCE)
 *   -> sum -> lookahead buffer -> 4x true-peak measurement -> linked gain reduction -> output
 *
 * Latency = lookahead samples + BS.1770 detector group delay (reported to host).
 */
#include <atomic>
#include <cmath>
#include <vector>

#include <juce_dsp/juce_dsp.h>

#include "Bs1770TruePeak.h"
#include "parameter_smoothing.h"

struct StemShieldMeterState {
  std::atomic<float> outputPeakDb { -100.0f };
  std::atomic<float> gainReductionDb { 0.0f };
};

class StemShieldDsp {
public:
  static constexpr float kDefaultCeilingDbTp = -0.5f;

  void prepare(const juce::dsp::ProcessSpec& spec);
  void reset();

  void setCeilingDbTp(float db) noexcept { ceilingDbTarget_ = db; }
  void setSubBalance(float norm01) noexcept { subBalanceTarget_ = norm01; }
  void setSubCrossoverHz(float hz) noexcept { crossoverTargetHz_ = hz; }
  void setReleaseMs(float ms) noexcept { releaseMsTarget_ = ms; }
  void setLookaheadMs(float ms) noexcept { lookaheadMsTarget_ = ms; }
  void setInputGainDb(float db) noexcept { inputGainDbTarget_ = db; }

  int getLatencySamples() const noexcept { return latencySamples_; }
  int getDetectorGroupDelaySamples() const noexcept { return detectorGroupDelay_; }
  int getLookaheadSamples() const noexcept { return lookaheadSamples_; }
  const Bs1770TruePeak& getTruePeakDetector() const noexcept { return truePeakDetector_; }

  StemShieldMeterState& getMeters() noexcept { return meters_; }
  const StemShieldMeterState& getMeters() const noexcept { return meters_; }

  void process(juce::AudioBuffer<float>& buffer);

private:
  float linkedTruePeakInRange(int startIndex, int endIndex, int writeIndex) const noexcept;

  juce::dsp::LinkwitzRileyFilter<float> crossover_;
  Bs1770TruePeak truePeakDetector_;

  std::vector<std::vector<float>> delayBuffer_;
  std::vector<float> limitedScratch_;
  int delaySize_ = 0;
  int writePos_ = 0;
  int lookaheadSamples_ = 0;
  int detectorGroupDelay_ = 0;
  int latencySamples_ = 0;

  DspParamSmooth ceilingSmooth_;
  DspParamSmooth subBalanceSmooth_;
  DspParamSmooth crossoverSmooth_;
  DspParamSmooth releaseSmooth_;
  DspParamSmooth lookaheadSmooth_;
  DspParamSmooth inputGainSmooth_;

  float ceilingDbTarget_ = kDefaultCeilingDbTp;
  float subBalanceTarget_ = 0.65f;
  float crossoverTargetHz_ = 80.0f;
  float releaseMsTarget_ = 120.0f;
  float lookaheadMsTarget_ = 2.5f;
  float inputGainDbTarget_ = 0.0f;

  float subLowEnv_ = 0.0f;
  float subFullEnv_ = 0.0f;

  float gainEnvelope_ = 1.0f;
  float blockOutputPeak_ = 0.0f;
  float blockGrDb_ = 0.0f;

  double sampleRate_ = 48000.0;
  int maxBlockSize_ = 512;
  int numChannels_ = 2;

  StemShieldMeterState meters_;
};
