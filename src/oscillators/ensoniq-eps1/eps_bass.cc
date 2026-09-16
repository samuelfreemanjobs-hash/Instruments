/*
 * Ensoniq EPS-1 inspired Memphis bass: wavetable digital grit + dark Curtis LP.
 */
#include "userosc.h"
#include "utils/fixed_math.h"
#include "dsp_utils.h"
#include "eps_bass.h"

#include <math.h>

#define EPS_FS 48000.0f
#define EPS_TWO_PI 6.28318530718f
#define EPS_AMP_FLOOR 0.00007f

enum eps_env_stage {
  k_eps_stage_idle = 0,
  k_eps_stage_attack,
  k_eps_stage_decay,
  k_eps_stage_sustain,
  k_eps_stage_release
};

static const int32_t s_wt_sine[EPS_WT_LEN] = {
#include "eps_wt_sine.inc"
};

static const int32_t s_wt_dark[EPS_WT_LEN] = {
#include "eps_wt_dark.inc"
};

static const int32_t s_wt_grit[EPS_WT_LEN] = {
#include "eps_wt_grit.inc"
};

static uint32_t s_phase;
static uint32_t s_rng;
static float s_drift;
static float s_drift_tgt;

static float s_w_cur;
static float s_w_tgt;

static uint8_t s_env_stage;
static uint8_t s_gate;
static float s_amp;
static float s_filt_env;

static float s_vibe;
static float s_wave;
static float s_drift_amt;
static float s_dark;
static float s_reso;
static float s_grit;
static float s_glide;
static float s_attack_sec;
static float s_tail_sec;
static float s_sustain_lvl;

static float s_lp1;
static float s_lp2;
static float s_lp3;
static float s_lp4;
static float s_grit_hold;
static uint32_t s_grit_ctr;

static float eps_norm100(uint16_t value) {
  if (value > 100U) {
    value = 100U;
  }
  return (float)value * 0.01f;
}

static float eps_time_sec(uint16_t value, float min_s, float max_s) {
  const float n = eps_norm100(value);
  return min_s + n * n * max_s;
}

static uint32_t eps_rand(void) {
  s_rng = s_rng * 1664525U + 1013904223U;
  return s_rng;
}

static uint32_t eps_float_to_w(float w) {
  if (w < 0.0f) {
    w = 0.0f;
  }
  if (w > 4294967295.0f) {
    w = 4294967295.0f;
  }
  return (uint32_t)w;
}

static int32_t eps_read_wt(const int32_t *a, const int32_t *b, uint32_t phase, float t) {
  const uint32_t idx = phase >> 26;
  const uint32_t idx_n = (idx + 1U) & (EPS_WT_LEN - 1U);
  const float frac = (float)(phase & 0x3FFFFFFU) / (float)0x4000000U;
  return dsp_lerp_q31(a[idx], a[idx_n], frac) * (1.0f - t) +
         dsp_lerp_q31(b[idx], b[idx_n], frac) * t;
}

static float eps_one_pole_g(float hz) {
  if (hz < 28.0f) {
    hz = 28.0f;
  }
  if (hz > 12000.0f) {
    hz = 12000.0f;
  }
  return 1.0f - expf(-EPS_TWO_PI * hz / EPS_FS);
}

static float eps_curtis_dark(float in, float cutoff_hz, float res_norm) {
  const float g = eps_one_pole_g(cutoff_hz);
  const float res = res_norm * (0.35f + s_vibe * 0.4f);
  const float fb = s_lp4 * (0.12f + res * 0.88f);
  float x = in - fb;

  s_lp1 += g * (x - s_lp1);
  s_lp2 += g * (s_lp1 - s_lp2);
  s_lp3 += g * (s_lp2 - s_lp3);
  s_lp4 += g * (s_lp3 - s_lp4);
  return s_lp4;
}

static void eps_update_sustain(void) {
  const float eerie = fminf(fmaxf((s_vibe - 0.35f) * 1.5f, 0.0f), 1.0f);
  s_sustain_lvl = 0.62f + eerie * 0.22f;
  if (s_sustain_lvl > 0.9f) {
    s_sustain_lvl = 0.9f;
  }
}

void eps_bass_reset(void) {
  s_phase = 0U;
  s_rng = 0xE951234U;
  s_drift = 0.0f;
  s_drift_tgt = 0.0f;
  s_w_cur = 0.0f;
  s_w_tgt = 0.0f;
  s_env_stage = k_eps_stage_idle;
  s_gate = 0U;
  s_amp = 0.0f;
  s_filt_env = 0.0f;
  s_vibe = eps_norm100(58U);
  s_wave = eps_norm100(45U);
  s_drift_amt = eps_norm100(48U);
  s_dark = eps_norm100(62U);
  s_reso = eps_norm100(42U);
  s_grit = eps_norm100(38U);
  s_glide = eps_norm100(28U);
  s_attack_sec = eps_time_sec(10U, 0.002f, 0.22f);
  s_tail_sec = eps_time_sec(52U, 0.05f, 1.8f);
  s_lp1 = 0.0f;
  s_lp2 = 0.0f;
  s_lp3 = 0.0f;
  s_lp4 = 0.0f;
  s_grit_hold = 0.0f;
  s_grit_ctr = 0U;
  eps_update_sustain();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  eps_bass_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  s_gate = 1U;
  s_w_tgt = (float)osc_w0(params->pitch);
  if (s_glide < 0.02f || s_w_cur < 1.0f) {
    s_w_cur = s_w_tgt;
  }
  s_phase = 0U;
  s_filt_env = 0.65f + s_reso * 0.25f;
  if (s_env_stage == k_eps_stage_idle || s_env_stage == k_eps_stage_release) {
    s_env_stage = k_eps_stage_attack;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
  if (s_env_stage != k_eps_stage_idle) {
    s_env_stage = k_eps_stage_release;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_eps_param_vibe:
      s_vibe = eps_norm100(value);
      eps_update_sustain();
      break;
    case k_eps_param_wave:
      s_wave = eps_norm100(value);
      break;
    case k_eps_param_drift:
      s_drift_amt = eps_norm100(value);
      break;
    case k_eps_param_dark:
      s_dark = eps_norm100(value);
      break;
    case k_eps_param_reso:
      s_reso = eps_norm100(value);
      break;
    case k_eps_param_grit:
      s_grit = eps_norm100(value);
      break;
    case k_eps_param_glide:
      s_glide = eps_norm100(value);
      break;
    case k_eps_param_attack:
      s_attack_sec = eps_time_sec(value, 0.002f, 0.22f);
      break;
    case k_eps_param_tail:
      s_tail_sec = eps_time_sec(value, 0.05f, 1.8f);
      break;
    default:
      break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  s_w_tgt = (float)osc_w0(params->pitch);

  const float glide_coef = 0.0001f + s_glide * s_glide * 0.007f;
  const float attack_step =
      (s_attack_sec > 0.0001f) ? (1.0f / (s_attack_sec * EPS_FS)) : 1.0f;
  const float decay_step =
      (s_tail_sec > 0.0001f) ? (1.0f / (s_tail_sec * EPS_FS)) : 1.0f;
  const float release_step = decay_step * 0.9f;

  const float host_cut = (float)params->cutoff * (1.0f / 8191.0f);
  const float host_res = (float)params->resonance * (1.0f / 8191.0f);
  const uint32_t grit_hold_len = 1U + (uint32_t)(s_grit * 12.0f);

  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    s_w_cur += (s_w_tgt - s_w_cur) * glide_coef;

    if ((eps_rand() & 0x1FFFU) < 14U) {
      const float r = (float)(eps_rand() & 0xFFFFU) * (1.0f / 32768.0f) - 1.0f;
      s_drift_tgt = r * s_drift_amt * (0.006f + s_vibe * 0.01f);
    }
    s_drift += (s_drift_tgt - s_drift) * 0.0004f;

    float w_f = s_w_cur * (1.0f + s_drift);
    const uint32_t w = eps_float_to_w(w_f);

    if (s_env_stage == k_eps_stage_attack) {
      s_amp += attack_step;
      s_filt_env += attack_step * 0.8f;
      if (s_amp >= 1.0f) {
        s_amp = 1.0f;
        s_filt_env = fminf(s_filt_env, 1.0f);
        s_env_stage = k_eps_stage_decay;
      }
    } else if (s_env_stage == k_eps_stage_decay) {
      s_amp += (s_sustain_lvl - s_amp) * decay_step;
      s_filt_env += (s_sustain_lvl * 0.45f - s_filt_env) * decay_step;
      if (fabsf(s_amp - s_sustain_lvl) < 0.002f) {
        s_amp = s_sustain_lvl;
        s_env_stage = k_eps_stage_sustain;
      }
    } else if (s_env_stage == k_eps_stage_sustain) {
      if (s_gate == 0U) {
        s_env_stage = k_eps_stage_release;
      }
      s_amp = s_sustain_lvl;
    } else if (s_env_stage == k_eps_stage_release) {
      s_amp -= release_step;
      s_filt_env -= release_step * 0.7f;
      if (s_amp < EPS_AMP_FLOOR) {
        s_amp = 0.0f;
        s_filt_env = 0.0f;
        s_env_stage = k_eps_stage_idle;
      }
    }

    float sig = 0.0f;
    if (s_env_stage != k_eps_stage_idle) {
      float morph = s_wave * 2.0f;
      int32_t wt;
      if (morph < 1.0f) {
        wt = eps_read_wt(s_wt_sine, s_wt_dark, s_phase, morph);
      } else {
        morph = morph - 1.0f;
        if (morph > 1.0f) {
          morph = 1.0f;
        }
        wt = eps_read_wt(s_wt_dark, s_wt_grit, s_phase, morph);
      }
      sig = (float)wt * (1.0f / 2147483648.0f);

      s_grit_ctr++;
      if (s_grit_ctr >= grit_hold_len) {
        s_grit_ctr = 0U;
        s_grit_hold = sig;
      }
      sig = sig * (1.0f - s_grit * 0.55f) + s_grit_hold * (s_grit * 0.55f);

      float cut = 70.0f + host_cut * (90.0f + (1.0f - s_dark) * 2200.0f);
      cut += s_filt_env * (200.0f + s_reso * 1400.0f);
      cut *= (1.0f - s_dark * 0.35f);
      const float res_use = s_reso * 0.65f + host_res * 0.35f;

      sig = eps_curtis_dark(sig, cut, res_use);
      sig *= s_amp * 0.9f;
    }

    s_phase += w;
    *y++ = f32_to_q31(sig);
  }
}
