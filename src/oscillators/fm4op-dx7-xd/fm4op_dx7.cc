/*
 * Minilogue XD — 4-operator FM oscillator (DX7 bank import planned).
 * v0: single built-in voice + panel params; P1 adds voices.bin from tools/dx7/.
 */
#include "userosc.h"
#include "dsp_utils.h"
#include "fm4op_dx7.h"

#include <math.h>

#define FM4_FS 48000.0f
#define FM4_TWO_PI 6.28318530718f

static uint32_t s_ph[4];
static float s_w0;
static float s_env[4];
static float s_amp;
static uint8_t s_gate;

static float s_patch_f;
static float s_algo_f;
static float s_level;
static float s_velo;
static float s_bright;
static float s_detune;

static Fm4OpVoice s_voice;

static float fm4_norm100(uint16_t v) {
  if (v > 100U) {
    v = 100U;
  }
  return (float)v * 0.01f;
}

static void fm4_load_default_voice(void) {
  s_voice.algorithm = 5U;
  s_voice.feedback[0] = 3U;
  for (int i = 0; i < 4; ++i) {
    s_voice.level[i] = 90;
    s_voice.ratio_coarse[i] = (i == 0) ? 1U : (i == 1) ? 14U : 1U;
    s_voice.ratio_fine[i] = 0U;
    s_voice.eg_rate[i] = 40U;
    s_voice.eg_level[i] = 99U;
  }
  s_voice.level[1] = 72;
}

static float fm4_sin(uint32_t ph) {
  const float n = (float)ph * (1.0f / 4294967296.0f);
  return sinf(FM4_TWO_PI * n);
}

static float fm4_op_ratio(const Fm4OpVoice &v, int op) {
  const float coarse = (float)v.ratio_coarse[op];
  const float fine = (float)v.ratio_fine[op] * 0.01f;
  return coarse + fine;
}

/* Algorithm 5-ish: op2→op1, op4→op3, sum (simplified 4-op stack). */
static float fm4_render_sample(void) {
  const float e0 = s_env[0] * (s_voice.level[0] / 99.0f);
  const float e1 = s_env[1] * (s_voice.level[1] / 99.0f);
  const float e2 = s_env[2] * (s_voice.level[2] / 99.0f);
  const float e3 = s_env[3] * (s_voice.level[3] / 99.0f);

  const float m2 = fm4_sin(s_ph[1]) * e1 * 3.0f * s_velo;
  const float m4 = fm4_sin(s_ph[3]) * e3 * 2.0f * s_velo;

  float car1 = fm4_sin(s_ph[0] + m2);
  float car2 = fm4_sin(s_ph[2] + m4);
  float sig = (car1 * e0 + car2 * e2) * 0.5f;

  const float fb = (float)s_voice.feedback[0] * 0.07f;
  sig += fm4_sin(s_ph[0] + sig * fb) * e0 * 0.25f;

  return sig * s_level * (0.4f + s_bright * 0.6f);
}

void fm4op_dx7_reset(void) {
  for (int i = 0; i < 4; ++i) {
    s_ph[i] = 0U;
    s_env[i] = 0.f;
  }
  s_w0 = 0.f;
  s_amp = 0.f;
  s_gate = 0U;
  s_patch_f = 0.f;
  s_algo_f = fm4_norm100(50U);
  s_level = fm4_norm100(85U);
  s_velo = fm4_norm100(80U);
  s_bright = fm4_norm100(55U);
  s_detune = fm4_norm100(50U);
  fm4_load_default_voice();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  fm4op_dx7_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 1U;
  s_amp = 1.f;
  for (int i = 0; i < 4; ++i) {
    s_env[i] = 1.f;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
  case k_fm4op_param_patch:
    s_patch_f = fm4_norm100(value);
    /* P1: index = (uint8_t)(s_patch_f * 31.99f) into embedded bank */
    break;
  case k_fm4op_param_algo:
    s_algo_f = fm4_norm100(value);
    s_voice.algorithm = (uint8_t)(s_algo_f * 39.f);
    break;
  case k_fm4op_param_level:
    s_level = fm4_norm100(value);
    break;
  case k_fm4op_param_velo:
    s_velo = 0.2f + fm4_norm100(value) * 0.8f;
    break;
  case k_fm4op_param_bright:
    s_bright = fm4_norm100(value);
    break;
  case k_fm4op_param_detune:
    s_detune = fm4_norm100(value);
    break;
  default:
    break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  const float det = 0.5f + (s_detune - 0.5f) * 0.04f;
  s_w0 = (float)osc_w0(params->pitch) * det;
  q31_t *y = (q31_t *)yn;

  const float env_dec = expf(-6.f / (0.15f * FM4_FS));

  for (uint32_t i = 0; i < frames; ++i) {
    if (s_gate) {
      s_amp = 1.f;
    } else {
      s_amp *= env_dec;
    }
    for (int op = 0; op < 4; ++op) {
      if (s_gate) {
        s_env[op] = 1.f;
      } else {
        s_env[op] *= env_dec;
      }
      const float w = s_w0 * fm4_op_ratio(s_voice, op);
      s_ph[op] += (uint32_t)(w * 4294967296.0f / FM4_FS);
    }

    float sig = fm4_render_sample() * s_amp;
    if (sig > 1.f) {
      sig = 1.f;
    }
    if (sig < -1.f) {
      sig = -1.f;
    }
    *y++ = f32_to_q31(sig);
  }
}
