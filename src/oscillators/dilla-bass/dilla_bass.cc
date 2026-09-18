/*
 * J Dilla inspired multifunction bass: glide, pitch wobble, warm MPC-ish tone.
 */
#include "userosc.h"
#include "utils/fixed_math.h"
#include "dsp_utils.h"
#include "dilla_bass.h"

#include <math.h>

#define DB_FS 48000.0f
#define DB_TWO_PI 6.28318530718f
#define DB_AMP_FLOOR 0.00008f

enum dilla_env_stage {
  k_dilla_stage_idle = 0,
  k_dilla_stage_attack,
  k_dilla_stage_decay,
  k_dilla_stage_sustain,
  k_dilla_stage_release
};

static uint32_t s_phase;
static uint32_t s_phase_sub;
static uint32_t s_lfo_a;
static uint32_t s_lfo_b;
static uint32_t s_rng;

static float s_w_cur;
static float s_w_tgt;
static float s_drift;
static float s_drift_tgt;

static uint8_t s_env_stage;
static uint8_t s_gate;
static float s_amp;
static float s_filt_env;

static float s_vibe;
static float s_glide;
static float s_wobble;
static float s_warmth;
static float s_grit;
static float s_attack_sec;
static float s_decay_sec;
static float s_release_sec;
static float s_thick;
static float s_sustain_lvl;

static float s_lp1;
static float s_lp2;
static float s_warm_z;

static float dilla_norm100(uint16_t value) {
  if (value > 100U) {
    value = 100U;
  }
  return (float)value * 0.01f;
}

static float dilla_time_sec(uint16_t value, float min_s, float max_s) {
  const float n = dilla_norm100(value);
  return min_s + n * n * max_s;
}

static uint32_t dilla_float_to_w(float w) {
  if (w < 0.0f) {
    w = 0.0f;
  }
  if (w > 4294967295.0f) {
    w = 4294967295.0f;
  }
  return (uint32_t)w;
}

static float dilla_saw_soft(uint32_t phase) {
  const float s = (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
  return tanhf(s * 0.85f);
}

static float dilla_sine(uint32_t phase) {
  const float norm = (float)phase * (1.0f / 4294967296.0f);
  return sinf(DB_TWO_PI * norm);
}

static float dilla_lfo(uint32_t ph) {
  const float norm = (float)ph * (1.0f / 4294967296.0f);
  return sinf(DB_TWO_PI * norm);
}

static uint32_t dilla_rand_u32(void) {
  s_rng = s_rng * 1664525U + 1013904223U;
  return s_rng;
}

static float dilla_one_pole_g(float hz) {
  if (hz < 25.0f) {
    hz = 25.0f;
  }
  if (hz > 12000.0f) {
    hz = 12000.0f;
  }
  return 1.0f - expf(-DB_TWO_PI * hz / DB_FS);
}

static float dilla_warm_lp(float in) {
  const float hz = 90.0f + (1.0f - s_warmth) * 2200.0f + s_warmth * 380.0f;
  const float g = dilla_one_pole_g(hz);
  s_warm_z += g * (in - s_warm_z);
  return in * (1.0f - s_warmth * 0.55f) + s_warm_z * (0.45f + s_warmth * 0.5f);
}

static float dilla_filter_lp(float in, float cutoff_hz, float res_norm) {
  const float g = dilla_one_pole_g(cutoff_hz);
  const float fb = s_lp2 * (0.05f + res_norm * 0.45f);
  const float x = in - fb;
  s_lp1 += g * (x - s_lp1);
  s_lp2 += g * (s_lp1 - s_lp2);
  return s_lp2;
}

static float dilla_grit_process(float in) {
  if (s_grit < 0.01f) {
    return in;
  }
  const float gain = 1.0f + s_grit * 2.8f;
  float x = tanhf(in * gain);
  return x * (1.0f - s_grit * 0.12f);
}

static void dilla_update_sustain(void) {
  const float pillow = 1.0f - fminf(s_vibe * 1.15f, 1.0f);
  const float gritty = fminf(fmaxf((s_vibe - 0.55f) * 2.0f, 0.0f), 1.0f);
  s_sustain_lvl = 0.62f * pillow + 0.78f * (1.0f - pillow) * 0.5f + gritty * 0.15f;
  if (s_sustain_lvl > 0.92f) {
    s_sustain_lvl = 0.92f;
  }
}

void dilla_bass_reset(void) {
  s_phase = 0U;
  s_phase_sub = 0U;
  s_lfo_a = 0U;
  s_lfo_b = 881233U;
  s_rng = 0xD1LLA123U;
  s_w_cur = 0.0f;
  s_w_tgt = 0.0f;
  s_drift = 0.0f;
  s_drift_tgt = 0.0f;
  s_env_stage = k_dilla_stage_idle;
  s_gate = 0U;
  s_amp = 0.0f;
  s_filt_env = 0.0f;
  s_vibe = dilla_norm100(32U);
  s_glide = dilla_norm100(48U);
  s_wobble = dilla_norm100(36U);
  s_warmth = dilla_norm100(58U);
  s_grit = dilla_norm100(28U);
  s_attack_sec = dilla_time_sec(14U, 0.002f, 0.28f);
  s_decay_sec = dilla_time_sec(40U, 0.04f, 1.2f);
  s_release_sec = dilla_time_sec(42U, 0.05f, 1.5f);
  s_thick = dilla_norm100(34U);
  s_lp1 = 0.0f;
  s_lp2 = 0.0f;
  s_warm_z = 0.0f;
  dilla_update_sustain();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  dilla_bass_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  s_gate = 1U;
  s_w_tgt = (float)osc_w0(params->pitch);
  if (s_glide < 0.02f || s_w_cur < 1.0f) {
    s_w_cur = s_w_tgt;
  }
  if (s_env_stage == k_dilla_stage_idle || s_env_stage == k_dilla_stage_release) {
    s_env_stage = k_dilla_stage_attack;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
  if (s_env_stage != k_dilla_stage_idle) {
    s_env_stage = k_dilla_stage_release;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_dilla_param_vibe:
      s_vibe = dilla_norm100(value);
      dilla_update_sustain();
      break;
    case k_dilla_param_glide:
      s_glide = dilla_norm100(value);
      break;
    case k_dilla_param_wobble:
      s_wobble = dilla_norm100(value);
      break;
    case k_dilla_param_warmth:
      s_warmth = dilla_norm100(value);
      break;
    case k_dilla_param_grit:
      s_grit = dilla_norm100(value);
      break;
    case k_dilla_param_attack:
      s_attack_sec = dilla_time_sec(value, 0.002f, 0.28f);
      break;
    case k_dilla_param_decay:
      s_decay_sec = dilla_time_sec(value, 0.04f, 1.2f);
      break;
    case k_dilla_param_release:
      s_release_sec = dilla_time_sec(value, 0.05f, 1.5f);
      break;
    case k_dilla_param_thick:
      s_thick = dilla_norm100(value);
      break;
    default:
      break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  s_w_tgt = (float)osc_w0(params->pitch);

  const float glide_coef = 0.0001f + s_glide * s_glide * 0.011f;
  const float attack_step =
      (s_attack_sec > 0.0001f) ? (1.0f / (s_attack_sec * DB_FS)) : 1.0f;
  const float decay_step =
      (s_decay_sec > 0.0001f) ? (1.0f / (s_decay_sec * DB_FS)) : 1.0f;
  const float release_step =
      (s_release_sec > 0.0001f) ? (1.0f / (s_release_sec * DB_FS)) : 1.0f;

  const float host_cut = (float)params->cutoff * (1.0f / 8191.0f);
  const float host_res = (float)params->resonance * (1.0f / 8191.0f);

  const uint32_t lfo_inc_a = 97349631U;
  const uint32_t lfo_inc_b = 147218953U;

  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    s_w_cur += (s_w_tgt - s_w_cur) * glide_coef;

    s_lfo_a += lfo_inc_a;
    s_lfo_b += lfo_inc_b;

    if ((dilla_rand_u32() & 0x3FFFU) < 18U) {
      const float r = (float)(dilla_rand_u32() & 0xFFFFU) * (1.0f / 32768.0f) - 1.0f;
      s_drift_tgt = r * s_wobble * (0.004f + s_vibe * 0.006f);
    }
    s_drift += (s_drift_tgt - s_drift) * 0.00035f;

    float w_f = s_w_cur * (1.0f + s_drift);
    w_f += s_w_cur * (dilla_lfo(s_lfo_a) * 0.5f + dilla_lfo(s_lfo_b) * 0.5f) *
           s_wobble * 0.0035f;
    const uint32_t w = dilla_float_to_w(w_f);
    const uint32_t w_sub = w >> 1;

    if (s_env_stage == k_dilla_stage_attack) {
      s_amp += attack_step;
      s_filt_env += attack_step * 0.75f;
      if (s_amp >= 1.0f) {
        s_amp = 1.0f;
        s_filt_env = fminf(s_filt_env, 1.0f);
        s_env_stage = k_dilla_stage_decay;
      }
    } else if (s_env_stage == k_dilla_stage_decay) {
      s_amp += (s_sustain_lvl - s_amp) * decay_step;
      s_filt_env += (s_sustain_lvl * 0.45f - s_filt_env) * decay_step;
      if (fabsf(s_amp - s_sustain_lvl) < 0.002f) {
        s_amp = s_sustain_lvl;
        s_env_stage = k_dilla_stage_sustain;
      }
    } else if (s_env_stage == k_dilla_stage_sustain) {
      if (s_gate == 0U) {
        s_env_stage = k_dilla_stage_release;
      }
      s_amp = s_sustain_lvl;
      s_filt_env = s_sustain_lvl * 0.45f;
    } else if (s_env_stage == k_dilla_stage_release) {
      s_amp -= release_step;
      s_filt_env -= release_step * 0.7f;
      if (s_amp < DB_AMP_FLOOR) {
        s_amp = 0.0f;
        s_filt_env = 0.0f;
        s_env_stage = k_dilla_stage_idle;
      }
    }

    float sig = 0.0f;
    if (s_env_stage != k_dilla_stage_idle) {
      const float sine = dilla_sine(s_phase);
      const float saw = dilla_saw_soft(s_phase);
      const float sub = dilla_sine(s_phase_sub);
      const float pillow = 1.0f - fminf(s_vibe * 1.1f, 1.0f);
      const float thick = s_thick * (0.35f + s_vibe * 0.45f);

      sig = sine * (0.72f + pillow * 0.2f);
      sig += saw * thick * (0.25f + s_vibe * 0.35f);
      sig += sub * thick * 0.42f;

      const float base_hz =
          55.0f + host_cut * (80.0f + pillow * 520.0f + s_vibe * 2400.0f);
      const float env_hz = base_hz + s_filt_env * (220.0f + s_vibe * 900.0f);
      const float res_use = host_res * (0.12f + pillow * 0.25f);

      sig = dilla_warm_lp(sig);
      sig = dilla_filter_lp(sig, env_hz, res_use);
      sig = dilla_grit_process(sig);
      sig *= s_amp * (0.9f + pillow * 0.06f);
    }

    s_phase += w;
    s_phase_sub += w_sub;
    *y++ = f32_to_q31(sig);
  }
}
