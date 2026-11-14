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

/** ~−0.5 dBFS peak ceiling for float osc output (use before DAC/q31 export). */
static inline float dsp_soft_limit_neg_half_dbfs(float x) {
  const float lim = 0.944060876f;
  if (x > lim) {
    return lim;
  }
  if (x < -lim) {
    return -lim;
  }
  const float abs_x = x < 0.0f ? -x : x;
  if (abs_x < 1.0e-20f) {
    return 0.0f;
  }
  return x;
}

#if defined(__userosc_h)
/**
 * Phase increment for v1.1.0-style oscillators (uint32 per sample).
 * Include userosc.h before dsp_utils.h.
 */
static inline uint32_t osc_w0(uint16_t pitch) {
  const float w0f =
      osc_w0f_for_note((uint8_t)(pitch >> 8), (uint8_t)(pitch & 0xFF));
  float scaled = w0f * 4294967296.0f;
  if (scaled > 4294967295.0f) {
    return 4294967295U;
  }
  return (uint32_t)scaled;
}
#endif

#ifdef __cplusplus
}
#endif
