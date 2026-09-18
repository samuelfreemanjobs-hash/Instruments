/*
 * Common DSP helpers for logue SDK user units.
 * Real-time safe: no allocation, bounded loops only.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** One-pole low-pass smoothing (float). coefficient in (0, 1]. */
static inline float dsp_smooth_one_pole(float state, float target, float coeff) {
  return state + (target - state) * coeff;
}

/** Map uint16_t param (0..65535) to float 0..1. */
static inline float dsp_param_to_norm(uint16_t value) {
  return (float)value * (1.0f / 65535.0f);
}

/** Map normalized float to uint32_t table index. */
static inline uint32_t dsp_norm_to_index(float norm, uint32_t table_len) {
  if (table_len <= 1U) {
    return 0U;
  }
  float scaled = norm * (float)(table_len - 1U);
  if (scaled < 0.0f) {
    scaled = 0.0f;
  }
  uint32_t idx = (uint32_t)scaled;
  if (idx >= table_len) {
    idx = table_len - 1U;
  }
  return idx;
}

/** Linear interpolation between two q31 samples. t in 0..1 as float. */
static inline int32_t dsp_lerp_q31(int32_t a, int32_t b, float t) {
  float fa = (float)a;
  float fb = (float)b;
  return (int32_t)(fa + (fb - fa) * t);
}

#ifdef __cplusplus
}
#endif
