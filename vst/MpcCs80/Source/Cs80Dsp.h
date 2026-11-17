#pragma once

#include <cmath>

namespace cs80 {

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

inline float applyPwm(float phase, float pwm) {
  const float width = 0.1f + pwm * 0.4f;
  return (phase < width) ? 1.0f : -1.0f;
}

} // namespace cs80