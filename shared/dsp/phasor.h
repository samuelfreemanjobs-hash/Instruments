#pragma once
/*
 * Unit phasor 0..1, float increment per sample.
 */
#include <cmath>

struct DspPhasor {
  float phase = 0.0f;
  float increment = 0.0f;

  void setFrequency(float hz, float sampleRate) {
    if (sampleRate <= 0.0f) {
      increment = 0.0f;
      return;
    }
    increment = hz / sampleRate;
  }

  void reset() { phase = 0.0f; }

  float next() {
    phase += increment;
    if (phase >= 1.0f) {
      phase -= 1.0f;
    }
    return phase;
  }

  static float sine(float phase01) {
    return std::sin(phase01 * 6.28318530718f);
  }
};
