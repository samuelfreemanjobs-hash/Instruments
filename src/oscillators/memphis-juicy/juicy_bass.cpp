/*
 * Juicy J / Three 6 Mafia Memphis sub: phase-locked sine, trunk rumble, tape mid.
 */
#include "userosc.h"
#include "utils/fixed_math.h"
#include "dsp_utils.h"
#include "juicy_bass.h"

#include <math.h>

#define JU_FS 48000.0f
#define JU_TWO_PI 6.28318530718f
#define JU_AMP_FLOOR 0.00006f

enum juicy_env_stage {
  k_juicy_stage_idle = 0,
  k_juicy_stage_attack,
  k_juicy_stage_decay,
  k_juicy_stage_sustain,
  k_juicy_stage_release
};

static uint32_t s_phase;
static uint32_t s_phase_deep;

static float s_w_cur;
static float s_w_tgt;

static uint8_t s_env_stage;
static uint8_t s_gate;
static float s_amp;
static float s_comp_env;

static float s_vibe;
static float s_sub_lvl;
static float s_trunk;
static float s_tape;
static float s_mid;
static float s_glide;
static float s_attack_sec;
static float s_decay_sec;
static float s_release_sec;
static float s_sustain_lvl;

static float s_sub_lp;
static float s_mid_hp_z;
static float s_tape_z;

static float juicy_norm100(uint16_t value) {
  if (value > 100U) {
    value = 100U;
  }
  return (float)value * 0.01f;
}

static float juicy_time_sec(uint16_t value, float min_s, float max_s) {
  const float n = juicy_norm100(value);
  return min_s + n * n * max_s;
}

static uint32_t juicy_float_to_w(float w) {
  if (w < 0.0f) {
    w = 0.0f;
  }
  if (w > 4294967295.0f) {
    w = 4294967295.0f;
  }
  return (uint32_t)w;
}

static float juicy_sin(uint32_t phase) {
  const float norm = (float)phase * (1.0f / 4294967296.0f);
  return sinf(JU_TWO_PI * norm);
}

static float juicy_one_pole_g(float hz) {
  if (hz < 20.0f) {
    hz = 20.0f;
  }
  if (hz > 12000.0f) {
    hz = 12000.0f;
  }
  return 1.0f - expf(-JU_TWO_PI * hz / JU_FS);
}

static float juicy_sub_lp(float in) {
  const float hz = 42.0f + (1.0f - s_trunk) * 55.0f;
  const float g = juicy_one_pole_g(hz);
  s_sub_lp += g * (in - s_sub_lp);
  return s_sub_lp;
}

static float juicy_mid_band(float sine) {
  if (s_mid < 0.01f) {
    return 0.0f;
  }
  float shaped = sine;
  const float drive = 1.0f + s_mid * (2.5f + s_vibe * 2.0f);
  shaped = tanhf(shaped * drive);
  shaped = shaped * shaped * (s_mid * 0.35f);
  const float g = juicy_one_pole_g(220.0f + s_mid * 480.0f);
  s_mid_hp_z += g * (shaped - s_mid_hp_z);
  return s_mid_hp_z * s_mid * (0.22f + s_vibe * 0.18f);
}

static float juicy_tape_compress(float in) {
  if (s_tape < 0.01f) {
    return in;
  }
  const float thresh = 0.55f - s_tape * 0.2f;
  float x = in;
  if (x > thresh) {
    x = thresh + (x - thresh) / (1.0f + s_tape * 4.0f);
  }
  if (x < -thresh) {
    x = -thresh + (x + thresh) / (1.0f + s_tape * 4.0f);
  }
  s_tape_z += 0.0012f * (tanhf(x * (1.0f + s_tape * 1.8f)) - s_tape_z);
  return in * (1.0f - s_tape * 0.25f) + s_tape_z * (s_tape * 0.65f);
}

static void juicy_update_sustain(void) {
  s_sustain_lvl = 0.82f + s_vibe * 0.08f;
  if (s_sustain_lvl > 0.95f) {
    s_sustain_lvl = 0.95f;
  }
}

void juicy_bass_reset(void) {
  s_phase = 0U;
  s_phase_deep = 0U;
  s_w_cur = 0.0f;
  s_w_tgt = 0.0f;
  s_env_stage = k_juicy_stage_idle;
  s_gate = 0U;
  s_amp = 0.0f;
  s_comp_env = 0.0f;
  s_vibe = juicy_norm100(55U);
  s_sub_lvl = juicy_norm100(78U);
  s_trunk = juicy_norm100(70U);
  s_tape = juicy_norm100(52U);
  s_mid = juicy_norm100(32U);
  s_glide = juicy_norm100(20U);
  s_attack_sec = juicy_time_sec(6U, 0.001f, 0.12f);
  s_decay_sec = juicy_time_sec(50U, 0.05f, 1.5f);
  s_release_sec = juicy_time_sec(48U, 0.05f, 1.4f);
  s_sub_lp = 0.0f;
  s_mid_hp_z = 0.0f;
  s_tape_z = 0.0f;
  juicy_update_sustain();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  juicy_bass_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  s_gate = 1U;
  s_w_tgt = (float)osc_w0(params->pitch);
  if (s_glide < 0.02f || s_w_cur < 1.0f) {
    s_w_cur = s_w_tgt;
  }
  if (s_env_stage == k_juicy_stage_idle || s_env_stage == k_juicy_stage_release) {
    s_env_stage = k_juicy_stage_attack;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
  if (s_env_stage != k_juicy_stage_idle) {
    s_env_stage = k_juicy_stage_release;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_juicy_param_vibe:
      s_vibe = juicy_norm100(value);
      juicy_update_sustain();
      break;
    case k_juicy_param_sub:
      s_sub_lvl = juicy_norm100(value);
      break;
    case k_juicy_param_trunk:
      s_trunk = juicy_norm100(value);
      break;
    case k_juicy_param_tape:
      s_tape = juicy_norm100(value);
      break;
    case k_juicy_param_mid:
      s_mid = juicy_norm100(value);
      break;
    case k_juicy_param_glide:
      s_glide = juicy_norm100(value);
      break;
    case k_juicy_param_attack:
      s_attack_sec = juicy_time_sec(value, 0.001f, 0.12f);
      break;
    case k_juicy_param_decay:
      s_decay_sec = juicy_time_sec(value, 0.05f, 1.5f);
      break;
    case k_juicy_param_release:
      s_release_sec = juicy_time_sec(value, 0.05f, 1.4f);
      break;
    default:
      break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  s_w_tgt = (float)osc_w0(params->pitch);

  const float glide_coef = 0.00012f + s_glide * s_glide * 0.005f;
  const float attack_step =
      (s_attack_sec > 0.0001f) ? (1.0f / (s_attack_sec * JU_FS)) : 1.0f;
  const float decay_step =
      (s_decay_sec > 0.0001f) ? (1.0f / (s_decay_sec * JU_FS)) : 1.0f;
  const float release_step =
      (s_release_sec > 0.0001f) ? (1.0f / (s_release_sec * JU_FS)) : 1.0f;

  const float host_cut = (float)params->cutoff * (1.0f / 8191.0f);

  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    s_w_cur += (s_w_tgt - s_w_cur) * glide_coef;
    const uint32_t w = juicy_float_to_w(s_w_cur);
    const uint32_t w_deep = w >> 1;

    if (s_env_stage == k_juicy_stage_attack) {
      s_amp += attack_step;
      s_comp_env += attack_step * 1.1f;
      if (s_amp >= 1.0f) {
        s_amp = 1.0f;
        s_comp_env = fminf(s_comp_env, 1.0f);
        s_env_stage = k_juicy_stage_decay;
      }
    } else if (s_env_stage == k_juicy_stage_decay) {
      s_amp += (s_sustain_lvl - s_amp) * decay_step;
      s_comp_env += (s_sustain_lvl - s_comp_env) * decay_step;
      if (fabsf(s_amp - s_sustain_lvl) < 0.002f) {
        s_amp = s_sustain_lvl;
        s_env_stage = k_juicy_stage_sustain;
      }
    } else if (s_env_stage == k_juicy_stage_sustain) {
      if (s_gate == 0U) {
        s_env_stage = k_juicy_stage_release;
      }
      s_amp = s_sustain_lvl;
      s_comp_env = s_sustain_lvl;
    } else if (s_env_stage == k_juicy_stage_release) {
      s_amp -= release_step;
      s_comp_env -= release_step;
      if (s_amp < JU_AMP_FLOOR) {
        s_amp = 0.0f;
        s_comp_env = 0.0f;
        s_env_stage = k_juicy_stage_idle;
      }
    }

    float sig = 0.0f;
    if (s_env_stage != k_juicy_stage_idle) {
      const float sine = juicy_sin(s_phase);
      const float deep = juicy_sin(s_phase_deep);

      float sub = sine * s_sub_lvl;
      sub += deep * s_trunk * (0.55f + s_sub_lvl * 0.25f);
      sub = juicy_sub_lp(sub);

      float mid = juicy_mid_band(sine * s_comp_env);
      sig = sub + mid;
      sig = juicy_tape_compress(sig);

      const float cut = 0.92f + host_cut * 0.08f;
      sig *= cut;
      sig *= s_amp * (0.88f + s_trunk * 0.08f);
    }

    s_phase += w;
    s_phase_deep += w_deep;
    *y++ = f32_to_q31(sig);
  }
}
