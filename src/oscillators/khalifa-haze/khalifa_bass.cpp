/*
 * Khalifa-style hazy stoner-rap sub: warm cloud bass that stays under bells/pads.
 */
#include "userosc.h"
#include "utils/fixed_math.h"
#include "dsp_utils.h"
#include "khalifa_bass.h"

#include <math.h>

#define KH_FS 48000.0f
#define KH_TWO_PI 6.28318530718f
#define KH_AMP_FLOOR 0.00006f

enum khalifa_env_stage {
  k_khalifa_stage_idle = 0,
  k_khalifa_stage_attack,
  k_khalifa_stage_decay,
  k_khalifa_stage_sustain,
  k_khalifa_stage_release
};

static uint32_t s_phase;
static uint32_t s_phase_h1;
static uint32_t s_phase_h2;
static uint32_t s_phase_deep;

static float s_w_cur;
static float s_w_tgt;

static uint8_t s_env_stage;
static uint8_t s_gate;
static float s_amp;

static float s_vibe;
static float s_sub_lvl;
static float s_haze;
static float s_float_amt;
static float s_warmth;
static float s_glide;
static float s_attack_sec;
static float s_decay_sec;
static float s_release_sec;
static float s_sustain_lvl;

static float s_lp1;
static float s_lp2;
static float s_lp3;
static float s_warm_z;

static float khalifa_norm100(uint16_t value) {
  if (value > 100U) {
    value = 100U;
  }
  return (float)value * 0.01f;
}

static float khalifa_time_sec(uint16_t value, float min_s, float max_s) {
  const float n = khalifa_norm100(value);
  return min_s + n * n * max_s;
}

static uint32_t khalifa_float_to_w(float w) {
  if (w < 0.0f) {
    w = 0.0f;
  }
  if (w > 4294967295.0f) {
    w = 4294967295.0f;
  }
  return (uint32_t)w;
}

static float khalifa_sin(uint32_t phase) {
  const float norm = (float)phase * (1.0f / 4294967296.0f);
  return sinf(KH_TWO_PI * norm);
}

static float khalifa_one_pole_g(float hz) {
  if (hz < 18.0f) {
    hz = 18.0f;
  }
  if (hz > 8000.0f) {
    hz = 8000.0f;
  }
  return 1.0f - expf(-KH_TWO_PI * hz / KH_FS);
}

static float khalifa_cloud_lp(float in, float hz) {
  const float g = khalifa_one_pole_g(hz);
  s_lp1 += g * (in - s_lp1);
  s_lp2 += g * (s_lp1 - s_lp2);
  s_lp3 += g * (s_lp2 - s_lp3);
  return s_lp3;
}

static float khalifa_warm(float in) {
  if (s_warmth < 0.01f) {
    return in;
  }
  const float g = 1.0f + s_warmth * 1.4f;
  s_warm_z += 0.0008f * (tanhf(in * g) - s_warm_z);
  return in * (1.0f - s_warmth * 0.35f) + s_warm_z * (s_warmth * 0.55f);
}

static void khalifa_update_sustain(void) {
  s_sustain_lvl = 0.75f + s_vibe * 0.12f;
  if (s_sustain_lvl > 0.92f) {
    s_sustain_lvl = 0.92f;
  }
}

void khalifa_bass_reset(void) {
  s_phase = 0U;
  s_phase_h1 = 0U;
  s_phase_h2 = 0U;
  s_phase_deep = 0U;
  s_w_cur = 0.0f;
  s_w_tgt = 0.0f;
  s_env_stage = k_khalifa_stage_idle;
  s_gate = 0U;
  s_amp = 0.0f;
  s_vibe = khalifa_norm100(50U);
  s_sub_lvl = khalifa_norm100(72U);
  s_haze = khalifa_norm100(55U);
  s_float_amt = khalifa_norm100(58U);
  s_warmth = khalifa_norm100(48U);
  s_glide = khalifa_norm100(55U);
  s_attack_sec = khalifa_time_sec(32U, 0.012f, 0.65f);
  s_decay_sec = khalifa_time_sec(52U, 0.06f, 1.8f);
  s_release_sec = khalifa_time_sec(62U, 0.08f, 2.4f);
  s_lp1 = 0.0f;
  s_lp2 = 0.0f;
  s_lp3 = 0.0f;
  s_warm_z = 0.0f;
  khalifa_update_sustain();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  khalifa_bass_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  s_gate = 1U;
  s_w_tgt = (float)osc_w0(params->pitch);
  if (s_glide < 0.02f || s_w_cur < 1.0f) {
    s_w_cur = s_w_tgt;
  }
  if (s_env_stage == k_khalifa_stage_idle || s_env_stage == k_khalifa_stage_release) {
    s_env_stage = k_khalifa_stage_attack;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
  if (s_env_stage != k_khalifa_stage_idle) {
    s_env_stage = k_khalifa_stage_release;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_khalifa_param_vibe:
      s_vibe = khalifa_norm100(value);
      khalifa_update_sustain();
      break;
    case k_khalifa_param_sub:
      s_sub_lvl = khalifa_norm100(value);
      break;
    case k_khalifa_param_haze:
      s_haze = khalifa_norm100(value);
      break;
    case k_khalifa_param_float:
      s_float_amt = khalifa_norm100(value);
      break;
    case k_khalifa_param_warmth:
      s_warmth = khalifa_norm100(value);
      break;
    case k_khalifa_param_glide:
      s_glide = khalifa_norm100(value);
      break;
    case k_khalifa_param_attack:
      s_attack_sec = khalifa_time_sec(value, 0.012f, 0.65f);
      break;
    case k_khalifa_param_decay:
      s_decay_sec = khalifa_time_sec(value, 0.06f, 1.8f);
      break;
    case k_khalifa_param_release:
      s_release_sec = khalifa_time_sec(value, 0.08f, 2.4f);
      break;
    default:
      break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  s_w_tgt = (float)osc_w0(params->pitch);

  const float glide_coef = 0.00006f + s_glide * s_glide * 0.0065f;
  const float attack_mul = 1.0f + s_float_amt * 0.85f;
  const float release_mul = 1.0f + s_float_amt * 1.2f;
  const float attack_step =
      (s_attack_sec > 0.0001f) ? (1.0f / (s_attack_sec * attack_mul * KH_FS)) : 1.0f;
  const float decay_step =
      (s_decay_sec > 0.0001f) ? (1.0f / (s_decay_sec * KH_FS)) : 1.0f;
  const float release_step =
      (s_release_sec > 0.0001f) ? (1.0f / (s_release_sec * release_mul * KH_FS)) : 1.0f;

  const float host_cut = (float)params->cutoff * (1.0f / 8191.0f);
  const float cloud = 1.0f - fminf(s_vibe * 0.25f, 0.25f);

  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    s_w_cur += (s_w_tgt - s_w_cur) * glide_coef;

    const uint32_t w = khalifa_float_to_w(s_w_cur);
    const uint32_t w_h1 = khalifa_float_to_w(s_w_cur * (1.0012f + s_haze * 0.0025f));
    const uint32_t w_h2 = khalifa_float_to_w(s_w_cur * (0.9988f - s_haze * 0.0025f));
    const uint32_t w_deep = w >> 1;

    if (s_env_stage == k_khalifa_stage_attack) {
      s_amp += attack_step;
      if (s_amp >= 1.0f) {
        s_amp = 1.0f;
        s_env_stage = k_khalifa_stage_decay;
      }
    } else if (s_env_stage == k_khalifa_stage_decay) {
      s_amp += (s_sustain_lvl - s_amp) * decay_step;
      if (fabsf(s_amp - s_sustain_lvl) < 0.002f) {
        s_amp = s_sustain_lvl;
        s_env_stage = k_khalifa_stage_sustain;
      }
    } else if (s_env_stage == k_khalifa_stage_sustain) {
      if (s_gate == 0U) {
        s_env_stage = k_khalifa_stage_release;
      }
      s_amp = s_sustain_lvl;
    } else if (s_env_stage == k_khalifa_stage_release) {
      s_amp -= release_step;
      if (s_amp < KH_AMP_FLOOR) {
        s_amp = 0.0f;
        s_env_stage = k_khalifa_stage_idle;
      }
    }

    float sig = 0.0f;
    if (s_env_stage != k_khalifa_stage_idle) {
      float core = khalifa_sin(s_phase);
      core += khalifa_sin(s_phase_h1) * s_haze * 0.22f;
      core += khalifa_sin(s_phase_h2) * s_haze * 0.2f;
      core += khalifa_sin(s_phase_deep) * s_sub_lvl * 0.35f * cloud;

      sig = core * s_sub_lvl * (0.72f + cloud * 0.15f);
      sig = khalifa_warm(sig);

      float cut_hz = 48.0f + host_cut * (60.0f + cloud * 180.0f);
      cut_hz += (1.0f - s_haze) * 80.0f;
      cut_hz -= s_vibe * 25.0f;
      if (cut_hz < 35.0f) {
        cut_hz = 35.0f;
      }
      sig = khalifa_cloud_lp(sig, cut_hz);
      sig *= s_amp * (0.82f + s_float_amt * 0.08f);
    }

    s_phase += w;
    s_phase_h1 += w_h1;
    s_phase_h2 += w_h2;
    s_phase_deep += w_deep;
    *y++ = f32_to_q31(sig);
  }
}
