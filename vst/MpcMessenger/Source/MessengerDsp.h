#pragma once

#include <algorithm>
#include <cmath>

namespace messenger {

inline float morphWave(float phase, float shape) {
  const float tri = 1.0f - 4.0f * std::abs(std::round(phase - 0.25f) - (phase - 0.25f));
  const float saw = 2.0f * (phase - std::floor(phase + 0.5f));
  const float sqr = (phase < 0.5f) ? 1.0f : -1.0f;
  if (shape < 0.5f) {
    const float t = shape * 2.0f;
    return tri * (1.0f - t) + saw * t;
  }
  const float t = (shape - 0.5f) * 2.0f;
  return saw * (1.0f - t) + sqr * t;
}

// Wavefold: foldAmount 0 = bypass, higher = more folds
inline float wavefold(float x, float foldAmount) {
  if (foldAmount <= 0.01f)
    return x;
  const float gain = 1.0f + foldAmount * 4.0f;
  x *= gain;
  const float limit = 1.0f + foldAmount;
  while (x > limit)
    x = 2.0f * limit - x;
  while (x < -limit)
    x = -2.0f * limit - x;
  return x / (1.0f + foldAmount * 0.5f);
}

// One-pole lowpass for res-bass dry path (compensation shelf proxy)
inline float lowpassOnePole(float in, float &state, float coeff) {
  state += coeff * (in - state);
  return state;
}

inline float resBassBlend(float filtered, float preFilterDry, float resonance,
                           float resBassKnob, float &bassLpState) {
  const float compAmt =
      std::clamp(resBassKnob * resonance, 0.0f, 1.0f);
  const float bassOnly =
      lowpassOnePole(preFilterDry, bassLpState, 0.08f); // ~low shelf proxy
  return filtered * (1.0f - compAmt * 0.35f) + bassOnly * compAmt;
}

} // namespace messenger