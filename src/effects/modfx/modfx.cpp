/*
 * Modulation effect — stereo chorus-lite (delay + cross-mix).
 */
#include "usermodfx.h"
#include "dsp_utils.h"
#include "modfx.h"

#define MODFX_DELAY_LEN 512U

static float s_delay_l[MODFX_DELAY_LEN];
static float s_delay_r[MODFX_DELAY_LEN];
static uint32_t s_write_pos;
static float s_depth;
static float s_depth_smooth;

void modfx_reset(void) {
  s_write_pos = 0U;
  s_depth = 0.25f;
  s_depth_smooth = s_depth;
  for (uint32_t i = 0; i < MODFX_DELAY_LEN; i++) {
    s_delay_l[i] = 0.0f;
    s_delay_r[i] = 0.0f;
  }
}

void MODFX_INIT(uint32_t platform, uint32_t api) {
  modfx_reset();
}

void MODFX_PARAM(uint8_t index, int32_t value) {
  if (index == 0U) {
    s_depth = dsp_param_to_norm((uint16_t)value) * 0.9f;
  }
}

void MODFX_PROCESS(const float *main_xn, float *main_yn,
                     const float *sub_xn, float *sub_yn,
                     uint32_t frames) {
  const uint32_t delay_samples = 48U;

  for (uint32_t i = 0; i < frames; i++) {
    s_depth_smooth = dsp_smooth_one_pole(s_depth_smooth, s_depth, 0.002f);

    const float in_l = main_xn[i];
    const float in_r = sub_xn[i];
    const uint32_t read_pos = (s_write_pos + MODFX_DELAY_LEN - delay_samples) % MODFX_DELAY_LEN;

    const float wet_l = s_delay_l[read_pos];
    const float wet_r = s_delay_r[read_pos];

    s_delay_l[s_write_pos] = in_l;
    s_delay_r[s_write_pos] = in_r;
    s_write_pos = (s_write_pos + 1U) % MODFX_DELAY_LEN;

    main_yn[i] = in_l + wet_r * s_depth_smooth;
    sub_yn[i] = in_r + wet_l * s_depth_smooth;
  }
}
