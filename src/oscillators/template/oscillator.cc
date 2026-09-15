/*
 * Template user oscillator — band-limited saw via phase accumulator (Q31).
 */
#include "userosc.h"
#include "oscillator.h"

static uint32_t s_phase;
static uint16_t s_shape;

void template_osc_reset(void) {
  s_phase = 0U;
  s_shape = 0U;
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  template_osc_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  (void)params;
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
}

void OSC_PARAM(uint16_t index, uint16_t value) {  // NOLINT: SDK v1.1 osc uses u16
  if (index == 0U) {
    s_shape = value;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  const uint32_t w = osc_w0(params->pitch + (s_shape >> 9));
  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    const q31_t out = (q31_t)(s_phase - 0x80000000U);
    *y++ = out;
    s_phase += w;
  }
}
