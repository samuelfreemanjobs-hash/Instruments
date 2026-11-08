/*
 * Delay effect — simple feedback delay line.
 */
#include "userdelfx.h"
#include "dsp_utils.h"
#include "delfx.h"

#define DELFX_BUF_LEN 12000U

static float s_buf[DELFX_BUF_LEN];
static uint32_t s_write_pos;
static float s_feedback;
static float s_feedback_smooth;
static float s_mix;
static float s_mix_smooth;

void delfx_reset(void) {
  s_write_pos = 0U;
  s_feedback = 0.35f;
  s_feedback_smooth = s_feedback;
  s_mix = 0.4f;
  s_mix_smooth = s_mix;
  for (uint32_t i = 0; i < DELFX_BUF_LEN; i++) {
    s_buf[i] = 0.0f;
  }
}

void DELFX_INIT(uint32_t platform, uint32_t api) {
  delfx_reset();
}

void DELFX_PARAM(uint8_t index, int32_t value) {
  const uint16_t uval = (uint16_t)value;
  if (index == 0U) {
    s_feedback = dsp_param_to_norm(uval) * 0.85f;
  } else if (index == 1U) {
    s_mix = dsp_param_to_norm(uval);
  }
}

static float process_sample(float dry) {
  const uint32_t delay_samples = 2400U;
  s_feedback_smooth = dsp_smooth_one_pole(s_feedback_smooth, s_feedback, 0.002f);
  s_mix_smooth = dsp_smooth_one_pole(s_mix_smooth, s_mix, 0.002f);

  const uint32_t read_pos =
      (s_write_pos + DELFX_BUF_LEN - delay_samples) % DELFX_BUF_LEN;
  const float delayed = s_buf[read_pos];
  const float wet = dry + delayed * s_feedback_smooth;

  s_buf[s_write_pos] = wet;
  s_write_pos = (s_write_pos + 1U) % DELFX_BUF_LEN;

  return dry * (1.0f - s_mix_smooth) + delayed * s_mix_smooth;
}

void DELFX_PROCESS(float *xn, uint32_t frames) {
  for (uint32_t i = 0; i < frames; i++) {
    const uint32_t base = i * 2U;
    xn[base] = process_sample(xn[base]);
    xn[base + 1U] = process_sample(xn[base + 1U]);
  }
}
