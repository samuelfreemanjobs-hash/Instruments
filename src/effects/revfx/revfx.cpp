/*
 * Reverb effect — lightweight comb-allpass network (Schroeder-style).
 */
#include "userrevfx.h"
#include "dsp_utils.h"
#include "revfx.h"

#define REVFX_COMB_LEN 2048U
#define REVFX_AP_LEN 512U

static float s_comb[REVFX_COMB_LEN];
static float s_ap[REVFX_AP_LEN];
static uint32_t s_comb_pos;
static uint32_t s_ap_pos;
static float s_room;
static float s_room_smooth;

void revfx_reset(void) {
  s_comb_pos = 0U;
  s_ap_pos = 0U;
  s_room = 0.5f;
  s_room_smooth = s_room;
  for (uint32_t i = 0; i < REVFX_COMB_LEN; i++) {
    s_comb[i] = 0.0f;
  }
  for (uint32_t i = 0; i < REVFX_AP_LEN; i++) {
    s_ap[i] = 0.0f;
  }
}

void REVFX_INIT(uint32_t platform, uint32_t api) {
  revfx_reset();
}

void REVFX_PARAM(uint8_t index, int32_t value) {
  if (index == 0U) {
    s_room = dsp_param_to_norm((uint16_t)value);
  }
}

static float process_sample(float dry) {
  const float comb_fb = 0.78f;
  const float ap_fb = 0.62f;
  const uint32_t comb_delay = 1553U;
  const uint32_t ap_delay = 347U;

  s_room_smooth = dsp_smooth_one_pole(s_room_smooth, s_room, 0.002f);

  const uint32_t comb_read =
      (s_comb_pos + REVFX_COMB_LEN - comb_delay) % REVFX_COMB_LEN;
  const float comb_out = s_comb[comb_read];
  s_comb[s_comb_pos] = dry + comb_out * comb_fb;
  s_comb_pos = (s_comb_pos + 1U) % REVFX_COMB_LEN;

  const uint32_t ap_read = (s_ap_pos + REVFX_AP_LEN - ap_delay) % REVFX_AP_LEN;
  const float ap_in = comb_out;
  const float ap_buf = s_ap[ap_read];
  const float ap_out = -ap_in + ap_buf;
  s_ap[s_ap_pos] = ap_in + ap_buf * ap_fb;
  s_ap_pos = (s_ap_pos + 1U) % REVFX_AP_LEN;

  return dry * (1.0f - s_room_smooth) + ap_out * s_room_smooth;
}

void REVFX_PROCESS(float *xn, uint32_t frames) {
  for (uint32_t i = 0; i < frames; i++) {
    const uint32_t base = i * 2U;
    xn[base] = process_sample(xn[base]);
    xn[base + 1U] = process_sample(xn[base + 1U]);
  }
}
