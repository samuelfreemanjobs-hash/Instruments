/*
 * Cardo "Got Wings" style late-night G-funk bass: sub layer + warm mid layer.
 */
#include "userosc.h"
#include "utils/fixed_math.h"
#include "dsp_utils.h"
#include "cardo_bass.h"

#include <math.h>

#define CD_FS 48000.0f
#define CD_TWO_PI 6.28318530718f
#define CD_AMP_FLOOR 0.00008f

enum cardo_env_stage {
  k_cardo_stage_idle = 0,
  k_cardo_stage_attack,
  k_cardo_stage_decay,
  k_cardo_stage_sustain,
  k_cardo_stage_release
};

static uint32_t s_phase_sub;
static uint32_t s_phase_mid;
static uint32_t s_phase_mid_det;

static float s_w_cur;
static float s_w_tgt;

static uint8_t s_env_stage;
static uint8_t s_gate;
static float s_amp;

static float s_vibe;
static float s_sub_lvl;
static float s_mid_lvl;
static float s_velvet;
static float s_glide;
static float s_glow;
static float s_attack_sec;
static float s_decay_sec;
static float s_release_sec;
static float s_sustain_lvl;

static float s_sub_lp;
static float s_mid_lp1;
static float s_mid_lp2;
static float s_chorus_z;
static uint32_t s_ch_wp;
static float s_ch_buf[384];

static float cardo_norm100(uint16_t value) {
  if (value > 100U) {
    value = 100U;
  }
  return (float)value * 0.01f;
}

static float cardo_time_sec(uint16_t value, float min_s, float max_s) {
  const float n = cardo_norm100(value);
  return min_s + n * n * max_s;
}

static uint32_t cardo_float_to_w(float w) {
  if (w < 0.0f) {
    w = 0.0f;
  }
  if (w > 4294967295.0f) {
    w = 4294967295.0f;
  }
  return (uint32_t)w;
}

static float cardo_sin(uint32_t phase) {
  const float norm = (float)phase * (1.0f / 4294967296.0f);
  return sinf(CD_TWO_PI * norm);
}

static float cardo_soft_saw(uint32_t phase) {
  const float s = (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
  return tanhf(s * 0.55f);
}

static float cardo_one_pole_g(float hz) {
  if (hz < 25.0f) {
    hz = 25.0f;
  }
  if (hz > 12000.0f) {
    hz = 12000.0f;
  }
  return 1.0f - expf(-CD_TWO_PI * hz / CD_FS);
}

static float cardo_glow(float in) {
  if (s_glow < 0.01f) {
    return in;
  }
  const float g = 1.0f + s_glow * 1.8f;
  return tanhf(in * g) * (1.0f / (1.0f + s_glow * 0.22f));
}

static float cardo_velvet_chorus(float in) {
  const float depth = s_velvet * 0.35f;
  if (depth < 0.02f) {
    return in;
  }
  const int32_t delay = 28 + (int32_t)(depth * 40.0f);
  const uint32_t read = (s_ch_wp + 384U - (uint32_t)delay) % 384U;
  const float wet = s_ch_buf[read];
  s_ch_buf[s_ch_wp] = in;
  s_ch_wp = (s_ch_wp + 1U) % 384U;
  return in * (1.0f - depth * 0.25f) + wet * depth * 0.25f;
}

static void cardo_update_sustain(void) {
  s_sustain_lvl = 0.72f + s_vibe * 0.18f;
  if (s_sustain_lvl > 0.94f) {
    s_sustain_lvl = 0.94f;
  }
}

void cardo_bass_reset(void) {
  s_phase_sub = 0U;
  s_phase_mid = 0U;
  s_phase_mid_det = 0U;
  s_w_cur = 0.0f;
  s_w_tgt = 0.0f;
  s_env_stage = k_cardo_stage_idle;
  s_gate = 0U;
  s_amp = 0.0f;
  s_vibe = cardo_norm100(48U);
  s_sub_lvl = cardo_norm100(68U);
  s_mid_lvl = cardo_norm100(38U);
  s_velvet = cardo_norm100(62U);
  s_glide = cardo_norm100(42U);
  s_glow = cardo_norm100(32U);
  s_attack_sec = cardo_time_sec(22U, 0.004f, 0.45f);
  s_decay_sec = cardo_time_sec(48U, 0.05f, 1.6f);
  s_release_sec = cardo_time_sec(52U, 0.06f, 2.0f);
  s_sub_lp = 0.0f;
  s_mid_lp1 = 0.0f;
  s_mid_lp2 = 0.0f;
  s_chorus_z = 0.0f;
  s_ch_wp = 0U;
  for (uint32_t i = 0; i < 384U; i++) {
    s_ch_buf[i] = 0.0f;
  }
  cardo_update_sustain();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  cardo_bass_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  s_gate = 1U;
  s_w_tgt = (float)osc_w0(params->pitch);
  if (s_glide < 0.02f || s_w_cur < 1.0f) {
    s_w_cur = s_w_tgt;
  }
  if (s_env_stage == k_cardo_stage_idle || s_env_stage == k_cardo_stage_release) {
    s_env_stage = k_cardo_stage_attack;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
  if (s_env_stage != k_cardo_stage_idle) {
    s_env_stage = k_cardo_stage_release;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_cardo_param_vibe:
      s_vibe = cardo_norm100(value);
      cardo_update_sustain();
      break;
    case k_cardo_param_sub:
      s_sub_lvl = cardo_norm100(value);
      break;
    case k_cardo_param_mid:
      s_mid_lvl = cardo_norm100(value);
      break;
    case k_cardo_param_velvet:
      s_velvet = cardo_norm100(value);
      break;
    case k_cardo_param_glide:
      s_glide = cardo_norm100(value);
      break;
    case k_cardo_param_glow:
      s_glow = cardo_norm100(value);
      break;
    case k_cardo_param_attack:
      s_attack_sec = cardo_time_sec(value, 0.004f, 0.45f);
      break;
    case k_cardo_param_decay:
      s_decay_sec = cardo_time_sec(value, 0.05f, 1.6f);
      break;
    case k_cardo_param_release:
      s_release_sec = cardo_time_sec(value, 0.06f, 2.0f);
      break;
    default:
      break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  s_w_tgt = (float)osc_w0(params->pitch);

  const float glide_coef = 0.00008f + s_glide * s_glide * 0.007f;
  const float attack_step =
      (s_attack_sec > 0.0001f) ? (1.0f / (s_attack_sec * CD_FS)) : 1.0f;
  const float decay_step =
      (s_decay_sec > 0.0001f) ? (1.0f / (s_decay_sec * CD_FS)) : 1.0f;
  const float release_step =
      (s_release_sec > 0.0001f) ? (1.0f / (s_release_sec * CD_FS)) : 1.0f;

  const float host_cut = (float)params->cutoff * (1.0f / 8191.0f);
  const float night = 1.0f - fminf(s_vibe * 0.35f, 0.35f);

  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    s_w_cur += (s_w_tgt - s_w_cur) * glide_coef;

    const uint32_t w_mid = cardo_float_to_w(s_w_cur);
    const uint32_t w_sub = w_mid >> 1;
    const uint32_t w_det = cardo_float_to_w(s_w_cur * (1.0035f + s_vibe * 0.002f));

    if (s_env_stage == k_cardo_stage_attack) {
      s_amp += attack_step;
      if (s_amp >= 1.0f) {
        s_amp = 1.0f;
        s_env_stage = k_cardo_stage_decay;
      }
    } else if (s_env_stage == k_cardo_stage_decay) {
      s_amp += (s_sustain_lvl - s_amp) * decay_step;
      if (fabsf(s_amp - s_sustain_lvl) < 0.002f) {
        s_amp = s_sustain_lvl;
        s_env_stage = k_cardo_stage_sustain;
      }
    } else if (s_env_stage == k_cardo_stage_sustain) {
      if (s_gate == 0U) {
        s_env_stage = k_cardo_stage_release;
      }
      s_amp = s_sustain_lvl;
    } else if (s_env_stage == k_cardo_stage_release) {
      s_amp -= release_step;
      if (s_amp < CD_AMP_FLOOR) {
        s_amp = 0.0f;
        s_env_stage = k_cardo_stage_idle;
      }
    }

    float sig = 0.0f;
    if (s_env_stage != k_cardo_stage_idle) {
      float sub = cardo_sin(s_phase_sub);
      const float g_sub = cardo_one_pole_g(58.0f + night * 40.0f);
      s_sub_lp += g_sub * (sub - s_sub_lp);
      sub = s_sub_lp * s_sub_lvl * (0.85f + s_vibe * 0.2f);

      float mid = cardo_soft_saw(s_phase_mid) * 0.55f;
      mid += cardo_soft_saw(s_phase_mid_det) * 0.45f;
      mid += cardo_sin(s_phase_mid) * 0.35f;

      const float mid_cut =
          120.0f + host_cut * (280.0f + s_velvet * 900.0f) + (1.0f - s_velvet) * 400.0f;
      const float g_mid = cardo_one_pole_g(mid_cut);
      s_mid_lp1 += g_mid * (mid - s_mid_lp1);
      s_mid_lp2 += g_mid * (s_mid_lp1 - s_mid_lp2);
      mid = s_mid_lp2 * s_mid_lvl * (0.5f + s_vibe * 0.35f);

      sig = sub + mid;
      sig = cardo_velvet_chorus(sig);
      sig = cardo_glow(sig);

      const float master_cut = 70.0f + host_cut * (180.0f + s_velvet * 2200.0f);
      const float g_m = cardo_one_pole_g(master_cut);
      s_chorus_z += g_m * (sig - s_chorus_z);
      sig = s_chorus_z;

      sig *= s_amp * 0.92f;
    }

    s_phase_sub += w_sub;
    s_phase_mid += w_mid;
    s_phase_mid_det += w_det;
    *y++ = f32_to_q31(sig);
  }
}
