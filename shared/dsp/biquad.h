#pragma once
/*
 * Transposed direct-form II biquad (RBJ lowpass).
 * Coefficients from Robert Bristow-Johnson Audio EQ Cookbook.
 */
#include <cmath>

struct DspBiquad {
  float b0 = 1.0f;
  float b1 = 0.0f;
  float b2 = 0.0f;
  float a1 = 0.0f;
  float a2 = 0.0f;
  float z1 = 0.0f;
  float z2 = 0.0f;

  void reset() {
    z1 = 0.0f;
    z2 = 0.0f;
  }

  void setLowpass(float sampleRate, float freqHz, float q) {
    if (sampleRate <= 0.0f) {
      return;
    }
    if (freqHz < 20.0f) {
      freqHz = 20.0f;
    }
    const float nyq = sampleRate * 0.5f;
    if (freqHz > nyq * 0.99f) {
      freqHz = nyq * 0.99f;
    }
    if (q < 0.1f) {
      q = 0.1f;
    }
    const float w0 = 6.28318530718f * freqHz / sampleRate;
    const float cosw0 = std::cos(w0);
    const float sinw0 = std::sin(w0);
    const float alpha = sinw0 / (2.0f * q);
    const float a0 = 1.0f + alpha;
    b0 = ((1.0f - cosw0) * 0.5f) / a0;
    b1 = (1.0f - cosw0) / a0;
    b2 = b0;
    a1 = (-2.0f * cosw0) / a0;
    a2 = (1.0f - alpha) / a0;
  }

  float process(float x) {
    const float y = b0 * x + z1;
    z1 = b1 * x - a1 * y + z2;
    z2 = b2 * x - a2 * y;
    return y;
  }
};
