#pragma once

/**
 * Memphis Grit — lo-fi distortion / vintage-sampler degradation (DiskLordz FX).
 *
 *   DRY ─────────────────────────────────────────┐
 *                                                 ├── MIX → OUTPUT trim → ceiling
 *   WET  → mid emphasis → drive (tanh) → bit crush → sample-hold decimation
 */
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include "parameter_smoothing.h"

class MemphisGritDsp {
public:
  static constexpr float kCeilingLinear = 0.944060876285209f; // -0.5 dBFS

  void prepare(double sampleRate, int maxBlockSize, int numChannels);
  void reset();

  void setDrive(float norm01) noexcept { driveTarget_ = norm01; }
  void setLofi(float norm01) noexcept { lofiTarget_ = norm01; }
  void setMid(float norm01) noexcept { midTarget_ = norm01; }
  void setMix(float norm01) noexcept { mixTarget_ = norm01; }
  void setOutputGainDb(float db) noexcept { outputGainDbTarget_ = db; }

  void process(juce::AudioBuffer<float>& buffer);

private:
  static float softClip(float x, float drive) noexcept;
  static float quantizeBits(float x, float lofiNorm) noexcept;
  static int holdInterval(float lofiNorm) noexcept;
  static float applyMidGrit(float x, float midNorm) noexcept;

  void processChannel(float* samples, int numSamples, float drive, float lofi, float mid,
                      int channel) noexcept;

  juce::AudioBuffer<float> dryBuffer_;

  DspParamSmooth driveSmooth_;
  DspParamSmooth lofiSmooth_;
  DspParamSmooth midSmooth_;
  DspParamSmooth mixSmooth_;
  DspParamSmooth gainSmooth_;

  float driveTarget_ = 0.35f;
  float lofiTarget_ = 0.4f;
  float midTarget_ = 0.5f;
  float mixTarget_ = 1.0f;
  float outputGainDbTarget_ = 0.0f;

  double sampleRate_ = 48000.0;
  int maxBlockSize_ = 512;
  int numChannels_ = 2;

  std::vector<int> holdCounters_;
  std::vector<float> holdValues_;
  std::vector<float> hpState_;
};
