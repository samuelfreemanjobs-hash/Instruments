/*
 * Yamaha DX7 "Lately Bass" style FM electric bass for NTS-1.
 */
#include "userosc.h"
#include "utils/fixed_math.h"
#include "dsp_utils.h"
#include "lately_bass.h"

#include <math.h>

#define LT_FS 48000.0f
#define LT_TWO_PI 6.28318530718f
#define LT_AMP_FLOOR 0.00008f

enum lately_env_stage {
  k_lately_stage_idle = 0,
  k_lately_stage_attack,
  k_lately_stage_decay,
  k_lately_stage_sustain,
  k_lately_stage_release
};

static uint32_t s_phase_c;
static uint32_t s_phase_m1;
static uint32_t s_phase_m2;

static float s_w_cur;
static float s_w_tgt;

static uint8_t s_env_stage;
static uint8_t s_gate;
static float s_amp;
static float s_mod_env;
static float s_mod_env2;

static float s_vibe;
static float s_index;
static float s_bite;
static float s_ratio;
static float s_glide;
static float s_attack_sec;
static float s_decay_sec;
static float s_release_sec;
static float s_tone;
static float s_sustain_lvl;

static float s_lp1;
static float s_lp2;

static float lately_norm100(uint16_t value) {
  if (value > 100U) {
    value = 100U;
  }
  return (float)value * 0.01f;
}

static float lately_time_sec(uint16_t value, float min_s, float max_s) {
  const float n = lately_norm100(value);
  return min_s + n * n * max_s;
}

static uint32_t lately_float_to_w(float w) {
  if (w < 0.0f) {
    w = 0.0f;
  }
  if (w > 4294967295.0f) {
    w = 4294967295.0f;
  }
  return (uint32_t)w;
}

static float lately_sin(uint32_t phase) {
  const float norm = (float)phase * (1.0f / 4294967296.0f);
  return sinf(LT_TWO_PI * norm);
}

static float lately_one_pole_g(float hz) {
  if (hz < 40.0f) {
    hz = 40.0f;
  }
  if (hz > 14000.0f) {
    hz = 14000.0f;
  }
  return 1.0f - expf(-LT_TWO_PI * hz / LT_FS);
}

static float lately_tone_lp(float in) {
  const float hz = 180.0f + (1.0f - s_tone) * 6500.0f + s_tone * 1200.0f;
  const float g = lately_one_pole_g(hz);
  s_lp1 += g * (in - s_lp1);
  s_lp2 += g * (s_lp1 - s_lp2);
  return s_lp2;
}

static void lately_update_sustain(void) {
  s_sustain_lvl = 0.48f + s_vibe * 0.38f;
  if (s_sustain_lvl > 0.9f) {
    s_sustain_lvl = 0.9f;
  }
}

void lately_bass_reset(void) {
  s_phase_c = 0U;
  s_phase_m1 = 0U;
  s_phase_m2 = 0U;
  s_w_cur = 0.0f;
  s_w_tgt = 0.0f;
  s_env_stage = k_lately_stage_idle;
  s_gate = 0U;
  s_amp = 0.0f;
  s_mod_env = 0.0f;
  s_mod_env2 = 0.0f;
  s_vibe = lately_norm100(52U);
  s_index = lately_norm100(62U);
  s_bite = lately_norm100(58U);
  s_ratio = lately_norm100(48U);
  s_glide = lately_norm100(22U);
  s_attack_sec = lately_time_sec(6U, 0.0005f, 0.12f);
  s_decay_sec = lately_time_sec(44U, 0.03f, 1.1f);
  s_release_sec = lately_time_sec(36U, 0.04f, 1.2f);
  s_tone = lately_norm100(55U);
  s_lp1 = 0.0f;
  s_lp2 = 0.0f;
  lately_update_sustain();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  lately_bass_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  s_gate = 1U;
  s_w_tgt = (float)osc_w0(params->pitch);
  if (s_glide < 0.02f || s_w_cur < 1.0f) {
    s_w_cur = s_w_tgt;
  }
  s_mod_env = 1.0f;
  s_mod_env2 = 0.85f;
  if (s_env_stage == k_lately_stage_idle || s_env_stage == k_lately_stage_release) {
    s_env_stage = k_lately_stage_attack;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
  if (s_env_stage != k_lately_stage_idle) {
    s_env_stage = k_lately_stage_release;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_lately_param_vibe:
      s_vibe = lately_norm100(value);
      lately_update_sustain();
      break;
    case k_lately_param_index:
      s_index = lately_norm100(value);
      break;
    case k_lately_param_bite:
      s_bite = lately_norm100(value);
      break;
    case k_lately_param_ratio:
      s_ratio = lately_norm100(value);
      break;
    case k_lately_param_glide:
      s_glide = lately_norm100(value);
      break;
    case k_lately_param_attack:
      s_attack_sec = lately_time_sec(value, 0.0005f, 0.12f);
      break;
    case k_lately_param_decay:
      s_decay_sec = lately_time_sec(value, 0.03f, 1.1f);
      break;
    case k_lately_param_release:
      s_release_sec = lately_time_sec(value, 0.04f, 1.2f);
      break;
    case k_lately_param_tone:
      s_tone = lately_norm100(value);
      break;
    default:
      break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  s_w_tgt = (float)osc_w0(params->pitch);

  const float glide_coef = 0.00014f + s_glide * s_glide * 0.008f;
  const float attack_step =
      (s_attack_sec > 0.0001f) ? (1.0f / (s_attack_sec * LT_FS)) : 1.0f;
  const float decay_step =
      (s_decay_sec > 0.0001f) ? (1.0f / (s_decay_sec * LT_FS)) : 1.0f;
  const float release_step =
      (s_release_sec > 0.0001f) ? (1.0f / (s_release_sec * LT_FS)) : 1.0f;

  const float bite_coef = 0.00008f + s_bite * s_bite * 0.0045f;
  const float bite2_coef = bite_coef * 0.35f;
  const float host_cut = (float)params->cutoff * (1.0f / 8191.0f);
  const float ratio_m1 = 1.0f + s_ratio * 1.85f;
  const float ratio_m2 = 2.0f + s_ratio * 2.5f;

  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    s_w_cur += (s_w_tgt - s_w_cur) * glide_coef;
    const uint32_t w_c = lately_float_to_w(s_w_cur);
    const uint32_t w_m1 = lately_float_to_w(s_w_cur * ratio_m1);
    const uint32_t w_m2 = lately_float_to_w(s_w_cur * ratio_m2);

    s_mod_env -= s_mod_env * bite_coef;
    s_mod_env2 -= s_mod_env2 * bite2_coef;
    if (s_mod_env < 0.02f) {
      s_mod_env = 0.02f;
    }
    if (s_mod_env2 < 0.04f) {
      s_mod_env2 = 0.04f;
    }

    if (s_env_stage == k_lately_stage_attack) {
      s_amp += attack_step;
      if (s_amp >= 1.0f) {
        s_amp = 1.0f;
        s_env_stage = k_lately_stage_decay;
      }
    } else if (s_env_stage == k_lately_stage_decay) {
      s_amp += (s_sustain_lvl - s_amp) * decay_step;
      if (fabsf(s_amp - s_sustain_lvl) < 0.002f) {
        s_amp = s_sustain_lvl;
        s_env_stage = k_lately_stage_sustain;
      }
    } else if (s_env_stage == k_lately_stage_sustain) {
      if (s_gate == 0U) {
        s_env_stage = k_lately_stage_release;
      }
      s_amp = s_sustain_lvl;
    } else if (s_env_stage == k_lately_stage_release) {
      s_amp -= release_step;
      if (s_amp < LT_AMP_FLOOR) {
        s_amp = 0.0f;
        s_env_stage = k_lately_stage_idle;
      }
    }

    float sig = 0.0f;
    if (s_env_stage != k_lately_stage_idle) {
      const float idx_base = s_index * (2.8f + s_vibe * 2.2f);
      const float idx1 = idx_base * s_mod_env;
      const float idx2 = idx_base * 0.42f * s_mod_env2;

      const float m1 = lately_sin(s_phase_m1);
      const float m2 = lately_sin(s_phase_m2);
      const float mod = m1 * idx1 + m2 * idx2;

      float car_phase = (float)s_phase_c * (1.0f / 4294967296.0f) * LT_TWO_PI;
      car_phase += mod * (0.85f + s_vibe * 0.25f);
      sig = sinf(car_phase);

      const float cut_boost = 0.65f + host_cut * 0.5f;
      sig = lately_tone_lp(sig * cut_boost);
      sig = tanhf(sig * (1.05f + s_vibe * 0.15f));
      sig *= s_amp * 0.92f;
    }

    s_phase_c += w_c;
    s_phase_m1 += w_m1;
    s_phase_m2 += w_m2;
    *y++ = f32_to_q31(sig);
  }
}
