#pragma once

/**
 * DiskLordz Phonk-Tape program chain.
 *
 * Signal path:
 *   Input (dry copy) -> modulated fractional-delay wow/flutter buffer
 *     -> tape saturation (4x polyphase IIR oversampling)
 *     -> AGE one-pole HF rolloff -> optional hiss (NOISE)
 *     -> dry/wet MIX -> OUTPUT trim -> -0.5 dBFS ceiling
 *
 * Wow/flutter uses a preallocated ring buffer per channel with linear
 * fractional-delay interpolation (read pointer trails write by LFO-modulated delay).
 * Reported host latency = fixed base delay + max LFO depth (+ oversampler latency).
 *
 * See README.md for design rationale.
 */
#include <juce_dsp/juce_dsp.h>

#include "parameter_smoothing.h"

class PhonkTapeDsp {
public:
  static constexpr float kCeilingLinear = 0.944060876285209f; // -0.5 dBFS

  void prepare(const juce::dsp::ProcessSpec& spec);
  void reset();

  void setDrive(float norm01) noexcept { driveTarget_ = norm01; }
  void setWow(float norm01) noexcept { wowTarget_ = norm01; }
  void setFlutter(float norm01) noexcept { flutterTarget_ = norm01; }
  void setAge(float norm01) noexcept { ageTarget_ = norm01; }
  void setNoise(float norm01) noexcept { noiseTarget_ = norm01; }
  void setMix(float norm01) noexcept { mixTarget_ = norm01; }
  void setOutputGainDb(float db) noexcept { outputGainDbTarget_ = db; }

  int getLatencySamples() const noexcept;
  void process(juce::AudioBuffer<float>& buffer);

private:
  static float tapeSaturate(float x, float drive) noexcept;
  static float readDelayLinear(const float* ring, int ringSize, float readIndex) noexcept;
  float wowFlutterDelaySamples(float wowSm, float flutterSm) const noexcept;

  std::unique_ptr<juce::dsp::Oversampling<float>> oversampling_;

  juce::AudioBuffer<float> dryBuffer_;
  juce::AudioBuffer<float> wetScratch_;
  std::vector<std::vector<float>> delayRing_;
  std::vector<float> writePos_;
  std::vector<float> ageLpState_;
  std::vector<uint32_t> noiseSeed_;

  int ringSize_ = 4096;
  float baseDelaySamples_ = 480.0f; // ~10 ms @ 48 kHz; set in prepare()
  float maxWowDepthSamples_ = 400.0f;
  float maxFlutterDepthSamples_ = 120.0f;

  float wowPhase_ = 0.0f;
  float flutterPhase_ = 0.0f;
  static constexpr float kWowHz = 0.72f;
  static constexpr float kFlutterHz = 7.8f;

  DspParamSmooth driveSmooth_;
  DspParamSmooth wowSmooth_;
  DspParamSmooth flutterSmooth_;
  DspParamSmooth ageSmooth_;
  DspParamSmooth noiseSmooth_;
  DspParamSmooth mixSmooth_;
  DspParamSmooth gainSmooth_;

  float driveTarget_ = 0.45f;
  float wowTarget_ = 0.35f;
  float flutterTarget_ = 0.4f;
  float ageTarget_ = 0.3f;
  float noiseTarget_ = 0.15f;
  float mixTarget_ = 1.0f;
  float outputGainDbTarget_ = 0.0f;

  double sampleRate_ = 48000.0;
  int maxBlockSize_ = 512;
  int numChannels_ = 2;
};
