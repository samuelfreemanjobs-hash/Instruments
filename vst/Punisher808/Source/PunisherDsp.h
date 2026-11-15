#pragma once

/**
 * DiskLordz 808-Punisher program chain.
 *
 * Signal path (mobile-translation design):
 *   Input -> Linkwitz-Riley 4th-order split @ SUB CROSSOVER
 *     * Low band: kept phase-coherent (fundamental sub)
 *     * High band: passed dry (mids/air untouched)
 *     * Harmonics: isolated low band -> 4x polyphase IIR oversampling -> blended
 *       even/odd waveshaper (2nd warmth vs 3rd bite) -> subtract fundamental so only
 *       overtones in ~2f–3f (e.g. 120–240 Hz when crossover is 60 Hz) are injected
 *     Sum -> dry/wet mix -> output trim -> -0.5 dBFS soft ceiling
 */
#include <juce_dsp/juce_dsp.h>

#include "parameter_smoothing.h"

class PunisherDsp {
public:
  static constexpr float kCeilingLinear = 0.944060876285209f; // -0.5 dBFS

  void prepare(const juce::dsp::ProcessSpec& spec);
  void reset();

  void setCrossoverHz(float hz) noexcept { crossoverTargetHz_ = hz; }
  void setPunish(float norm01) noexcept { punishTarget_ = norm01; }
  void setHarmonicShape(float norm01) noexcept { shapeTarget_ = norm01; }
  void setMix(float norm01) noexcept { mixTarget_ = norm01; }
  void setOutputGainDb(float db) noexcept { outputGainDbTarget_ = db; }

  int getLatencySamples() const noexcept;
  void process(juce::AudioBuffer<float>& buffer);

private:
  static float waveshapeHarmonic(float low, float punish, float shape) noexcept;

  juce::dsp::LinkwitzRileyFilter<float> crossover_;
  std::unique_ptr<juce::dsp::Oversampling<float>> oversampling_;

  juce::AudioBuffer<float> dryBuffer_;
  juce::AudioBuffer<float> lowBandBuffer_;
  juce::AudioBuffer<float> highBandBuffer_;
  juce::AudioBuffer<float> harmBuffer_;

  DspParamSmooth punishSmooth_;
  DspParamSmooth shapeSmooth_;
  DspParamSmooth mixSmooth_;
  DspParamSmooth gainSmooth_;
  DspParamSmooth crossoverSmooth_;

  float punishTarget_ = 0.55f;
  float shapeTarget_ = 0.7f;
  float mixTarget_ = 1.0f;
  float outputGainDbTarget_ = 0.0f;
  float crossoverTargetHz_ = 60.0f;

  double sampleRate_ = 48000.0;
  int maxBlockSize_ = 512;
  int numChannels_ = 2;
};
