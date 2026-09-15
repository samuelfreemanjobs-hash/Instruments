/*
 * Wavetable oscillator with morph between sine and saw tables.
 */
#include "userosc.h"
#include "dsp_utils.h"
#include "wavetable.h"

static const int32_t s_sine_table[WAVETABLE_SIZE] = {
#include "wavetable_sine.inc"
};

static const int32_t s_saw_table[WAVETABLE_SIZE] = {
#include "wavetable_saw.inc"
};

static uint32_t s_phase;
static float s_morph;

void wavetable_osc_reset(void) {
  s_phase = 0U;
  s_morph = 0.0f;
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  wavetable_osc_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  (void)params;
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  if (index == 0U) {
    s_morph = dsp_param_to_norm(value);
  }
}

static int32_t read_wavetable(const int32_t *table, uint32_t phase) {
  const uint32_t idx = phase >> 24;
  const uint32_t idx_next = (idx + 1U) & (WAVETABLE_SIZE - 1U);
  const float frac = (float)(phase & 0xFFFFFFU) * (1.0f / 16777216.0f);
  return dsp_lerp_q31(table[idx], table[idx_next], frac);
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  const uint32_t w = osc_w0(params->pitch);
  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    const int32_t a = read_wavetable(s_sine_table, s_phase);
    const int32_t b = read_wavetable(s_saw_table, s_phase);
    *y++ = dsp_lerp_q31(a, b, s_morph);
    s_phase += w;
  }
}
