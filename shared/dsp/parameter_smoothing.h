#pragma once
/*
 * Portable float parameter smoothing (one-pole).
 * Safe for realtime audio — no allocation.
 */
#include <cmath>

struct DspParamSmooth {
  float state = 0.0f;
  float coeff = 1.0f;

  /** coeff in (0, 1]; higher = faster tracking. */
  static float coeffFromMs(float smoothingMs, float sampleRate) {
    if (smoothingMs <= 0.0f || sampleRate <= 0.0f) {
      return 1.0f;
    }
    const float tau = smoothingMs * 0.001f;
    return 1.0f - std::exp(-1.0f / (tau * sampleRate));
  }

  void setTargetCoeff(float smoothingMs, float sampleRate) {
    coeff = coeffFromMs(smoothingMs, sampleRate);
  }

  void reset(float value) { state = value; }

  float process(float target) {
    state += (target - state) * coeff;
    return state;
  }
};
