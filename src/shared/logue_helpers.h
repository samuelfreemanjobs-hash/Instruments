/*
 * logue SDK convenience macros and parameter scaling.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOGUE_CLAMP_U16(v, lo, hi) \
  ((uint16_t)(((v) < (lo)) ? (lo) : (((v) > (hi)) ? (hi) : (v))))

/** Scale manifest knob range to 0..1 float. */
static inline float logue_knob_norm(uint16_t value, uint16_t min_v, uint16_t max_v) {
  if (max_v <= min_v) {
    return 0.0f;
  }
  uint16_t clamped = LOGUE_CLAMP_U16(value, min_v, max_v);
  return (float)(clamped - min_v) / (float)(max_v - min_v);
}

#ifdef __cplusplus
}
#endif
