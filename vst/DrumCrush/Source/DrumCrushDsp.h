#pragma once

/**
 * DiskLordz Drum-Crush — parallel bus transient punch & brickwall soft-clipper.
 *
 * Design (Phase 2 / KNOCK-class drum bus macro):
 *
 *   DRY  ────────────────────────────────────────────────┐
 *                                                        ├── MIX → OUTPUT trim → -0.5 dBFS ceiling
 *   WET  Input → transient punch (PUNCH)                 │
 *              → high-shelf snap (coupled to PUNCH)       │
 *              → 4× oversampled saturation (CRUSH)        │
 *              → soft-clip waveshape (CLIP)               │
 *              → peak limiter (inter-sample aware gain)   ┘
 *
 * Knobs:
 *   PUNCH  — emphasises attack/transient vs body (envelope Δ + shelf amount)
 *   CRUSH  — parallel-path harmonic density (post-shelf drive, anti-aliased)
 *   CLIP   — brickwall-soft knee before limiter tail (tape-style ceiling glue)
 *   MIX    — dry/wet parallel bus (100% = full crushed wet chain)
 *   OUTPUT — post-mix trim (dB), then fixed -0.5 dBFS safety ceiling
 */
#include <juce_dsp/juce_dsp.h>

#include "parameter_smoothing.h"

class DrumCrushDsp {
public:
  static constexpr float kCeilingLinear = 0.944060876285209f; // -0.5 dBFS

  void prepare(const juce::dsp::ProcessSpec& spec);
  void reset();

  void setPunch(float norm01) noexcept { punchTarget_ = norm01; }
  void setCrush(float norm01) noexcept { crushTarget_ = norm01; }
  void setClip(float norm01) noexcept { clipTarget_ = norm01; }
  void setMix(float norm01) noexcept { mixTarget_ = norm01; }
  void setOutputGainDb(float db) noexcept { outputGainDbTarget_ = db; }

  int getLatencySamples() const noexcept;
  void process(juce::AudioBuffer<float>& buffer);

private:
  static float applySoftClip(float x, float clipNorm) noexcept;
  static float saturateCrush(float x, float crushNorm) noexcept;

  void processWetChannel(float* samples, int numSamples, int channel, float punch, float crush,
                         float clip) noexcept;

  std::unique_ptr<juce::dsp::Oversampling<float>> oversampling_;
  juce::dsp::Limiter<float> peakLimiter_;

  juce::AudioBuffer<float> dryBuffer_;
  juce::AudioBuffer<float> wetBuffer_;

  DspParamSmooth punchSmooth_;
  DspParamSmooth crushSmooth_;
  DspParamSmooth clipSmooth_;
  DspParamSmooth mixSmooth_;
  DspParamSmooth gainSmooth_;

  float punchTarget_ = 0.5f;
  float crushTarget_ = 0.45f;
  float clipTarget_ = 0.55f;
  float mixTarget_ = 1.0f;
  float outputGainDbTarget_ = 0.0f;

  double sampleRate_ = 48000.0;
  int maxBlockSize_ = 512;
  int numChannels_ = 2;

  std::vector<float> envFast_;
  std::vector<float> envSlow_;
  std::vector<float> shelfLowpass_;
};
