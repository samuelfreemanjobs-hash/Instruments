#pragma once

#include <cmath>

namespace subphatty {

inline float morphWave(float phase, float shape) {
  const float tri = 1.0f - 4.0f * std::abs(std::round(phase - 0.25f) - (phase - 0.25f));
  const float saw = 2.0f * (phase - std::floor(phase + 0.5f));
  const float sqr = (phase < 0.5f) ? 1.0f : -1.0f;
  const float narrowPulse = (phase < 0.18f) ? 1.0f : -1.0f;

  if (shape < 0.33f) {
    const float t = shape / 0.33f;
    return tri * (1.0f - t) + saw * t;
  }
  if (shape < 0.66f) {
    const float t = (shape - 0.33f) / 0.33f;
    return saw * (1.0f - t) + sqr * t;
  }
  const float t = (shape - 0.66f) / 0.34f;
  return sqr * (1.0f - t) + narrowPulse * t;
}

inline float applyMultidrive(float x, float driveAmount) {
  if (driveAmount <= 0.01f)
    return x;
  const float gain = 1.0f + driveAmount * 7.5f;
  const float driven = x * gain;
  const float pos = std::tanh(driven * 1.15f);
  const float neg = std::tanh(driven * 0.85f);
  const float out = (driven >= 0.0f) ? pos : neg;
  return out / (1.0f + driveAmount * 0.65f);
}

} // namespace subphatty