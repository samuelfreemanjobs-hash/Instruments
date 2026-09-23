/*
 * Minilogue XD — FM6OSC-style 6-operator FM (DX7 bank import planned).
 * v0: Lately-style default ratios + serial FM stack; P2: full DX7 algo router.
 */
#include "userosc.h"
#include "dsp_utils.h"
#include "fm6op_dx7.h"

#include <math.h>

#define FM6_FS 48000.0f
#define FM6_TWO_PI 6.28318530718f
#define FM6_NUM_OPS 6

static uint32_t s_ph[FM6_NUM_OPS];
static float s_w0;
static float s_env[FM6_NUM_OPS];
static float s_amp;
static uint8_t s_gate;

static float s_patch_f;
static float s_algo_f;
static float s_level;
static float s_velo;
static float s_bright;
static float s_detune;

static Fm6OpVoice s_voice;

static float fm6_norm100(uint16_t v) {
  if (v > 100U) {
    v = 100U;
  }
  return (float)v * 0.01f;
}

static void fm6_load_default_voice(void) {
  /* DX7-ish "FM electric bass" starting point (6 ops active). */
  s_voice.algorithm = 5U;
  const uint8_t coarse[FM6_NUM_OPS] = {1, 14, 1, 7, 1, 3};
  const int8_t lvl[FM6_NUM_OPS] = {99, 78, 0, 55, 0, 40};
  for (int i = 0; i < FM6_NUM_OPS; ++i) {
    s_voice.feedback[i] = (i == 0) ? 4U : 0U;
    s_voice.level[i] = lvl[i];
    s_voice.ratio_coarse[i] = coarse[i];
    s_voice.ratio_fine[i] = 0U;
    s_voice.eg_rate[i] = 38U;
    s_voice.eg_level[i] = 99U;
  }
}

static float fm6_sin(uint32_t ph) {
  const float n = (float)ph * (1.0f / 4294967296.0f);
  return sinf(FM6_TWO_PI * n);
}

static float fm6_op_ratio(const Fm6OpVoice &v, int op) {
  return (float)v.ratio_coarse[op] + (float)v.ratio_fine[op] * 0.01f;
}

static float fm6_op_env(int op) {
  return s_env[op] * (s_voice.level[op] / 99.0f);
}

/* DX7 algorithm 5 simplified: 2←1, 4←3, 6←5, out = 1+3+5 */
static float fm6_algo5(void) {
  const float e1 = fm6_op_env(0);
  const float e2 = fm6_op_env(1);
  const float e3 = fm6_op_env(2);
  const float e4 = fm6_op_env(3);
  const float e5 = fm6_op_env(4);
  const float e6 = fm6_op_env(5);

  const float m6 = fm6_sin(s_ph[5]) * e6 * 2.5f * s_velo;
  const float m4 = fm6_sin(s_ph[3]) * e4 * 2.2f * s_velo;
  const float m2 = fm6_sin(s_ph[1]) * e2 * 3.0f * s_velo;

  float op5 = fm6_sin(s_ph[4] + m6);
  float op3 = fm6_sin(s_ph[2] + m4);
  float op1 = fm6_sin(s_ph[0] + m2);

  const float fb = (float)s_voice.feedback[0] * 0.06f;
  op1 = fm6_sin(s_ph[0] + m2 + op1 * fb);

  return (op1 * e1 + op3 * e3 + op5 * e5) * 0.33f;
}

static float fm6_render_sample(void) {
  const uint8_t algo = (uint8_t)(s_algo_f * 31.99f);
  if (algo == 5U || s_voice.algorithm == 5U) {
    return fm6_algo5();
  }
  /* Fallback: parallel sines (algo 0 proxy) */
  float sum = 0.f;
  for (int op = 0; op < FM6_NUM_OPS; ++op) {
    sum += fm6_sin(s_ph[op]) * fm6_op_env(op);
  }
  return sum * 0.2f;
}

void fm6op_dx7_reset(void) {
  for (int i = 0; i < FM6_NUM_OPS; ++i) {
    s_ph[i] = 0U;
    s_env[i] = 0.f;
  }
  s_w0 = 0.f;
  s_amp = 0.f;
  s_gate = 0U;
  s_patch_f = 0.f;
  s_algo_f = fm6_norm100(50U);
  s_level = fm6_norm100(85U);
  s_velo = fm6_norm100(80U);
  s_bright = fm6_norm100(55U);
  s_detune = fm6_norm100(50U);
  fm6_load_default_voice();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;
  fm6op_dx7_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 1U;
  s_amp = 1.f;
  for (int i = 0; i < FM6_NUM_OPS; ++i) {
    s_env[i] = 1.f;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
  case k_fm6op_param_patch:
    s_patch_f = fm6_norm100(value);
    break;
  case k_fm6op_param_algo:
    s_algo_f = fm6_norm100(value);
    s_voice.algorithm = (uint8_t)(s_algo_f * 31.f);
    break;
  case k_fm6op_param_level:
    s_level = fm6_norm100(value);
    break;
  case k_fm6op_param_velo:
    s_velo = 0.2f + fm6_norm100(value) * 0.8f;
    break;
  case k_fm6op_param_bright:
    s_bright = fm6_norm100(value);
    break;
  case k_fm6op_param_detune:
    s_detune = fm6_norm100(value);
    break;
  default:
    break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  const float det = 0.5f + (s_detune - 0.5f) * 0.04f;
  s_w0 = (float)osc_w0(params->pitch) * det;
  q31_t *y = (q31_t *)yn;

  const float env_dec = expf(-6.f / (0.18f * FM6_FS));

  for (uint32_t i = 0; i < frames; ++i) {
    if (s_gate) {
      s_amp = 1.f;
    } else {
      s_amp *= env_dec;
    }
    for (int op = 0; op < FM6_NUM_OPS; ++op) {
      if (s_gate) {
        s_env[op] = 1.f;
      } else {
        s_env[op] *= env_dec;
      }
      const float w = s_w0 * fm6_op_ratio(s_voice, op);
      s_ph[op] += (uint32_t)(w * 4294967296.0f / FM6_FS);
    }

    float sig = fm6_render_sample() * s_amp * s_level * (0.4f + s_bright * 0.6f);
    if (sig > 1.f) {
      sig = 1.f;
    }
    if (sig < -1.f) {
      sig = -1.f;
    }
    *y++ = f32_to_q31(sig);
  }
}
