/*
 * Roland SH-101 "Babyface" style: warm PWM pulse, sub, hollow 2-op FM bass.
 */
#include "userosc.h"
#include "utils/fixed_math.h"
#include "dsp_utils.h"
#include "sh101_bass.h"

#include <math.h>

#define SH_FS 48000.0f
#define SH_TWO_PI 6.28318530718f
#define SH_AMP_FLOOR 0.00008f

enum sh101_env_stage {
  k_sh101_stage_idle = 0,
  k_sh101_stage_attack,
  k_sh101_stage_decay,
  k_sh101_stage_sustain,
  k_sh101_stage_release
};

static uint32_t s_phase;
static uint32_t s_phase_sub;
static uint32_t s_phase_mod;
static uint32_t s_pwm_lfo;

static float s_w_cur;
static float s_w_tgt;

static uint8_t s_env_stage;
static uint8_t s_gate;
static float s_amp;
static float s_filt_env;
static float s_fm_env;

static float s_vibe;
static float s_pwm_depth;
static float s_sub_lvl;
static float s_hollow;
static float s_saw_mix;
static float s_glide;
static float s_attack_sec;
static float s_decay_sec;
static float s_release_sec;
static float s_sustain_lvl;

static float s_lp1;
static float s_lp2;
static float s_lp3;
static float s_lp4;

static float sh101_norm100(uint16_t value) {
  if (value > 100U) {
    value = 100U;
  }
  return (float)value * 0.01f;
}

static float sh101_time_sec(uint16_t value, float min_s, float max_s) {
  const float n = sh101_norm100(value);
  return min_s + n * n * max_s;
}

static uint32_t sh101_float_to_w(float w) {
  if (w < 0.0f) {
    w = 0.0f;
  }
  if (w > 4294967295.0f) {
    w = 4294967295.0f;
  }
  return (uint32_t)w;
}

static float sh101_saw(uint32_t phase) {
  return (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
}

static float sh101_pulse(uint32_t phase, float width) {
  if (width < 0.1f) {
    width = 0.1f;
  }
  if (width > 0.9f) {
    width = 0.9f;
  }
  const uint32_t th = (uint32_t)(width * 4294967295.0f);
  return (phase < th) ? 1.0f : -1.0f;
}

static float sh101_sin(uint32_t phase) {
  const float norm = (float)phase * (1.0f / 4294967296.0f);
  return sinf(SH_TWO_PI * norm);
}

static float sh101_lfo(uint32_t ph) {
  const float norm = (float)ph * (1.0f / 4294967296.0f);
  return sinf(SH_TWO_PI * norm);
}

static float sh101_one_pole_g(float hz) {
  if (hz < 35.0f) {
    hz = 35.0f;
  }
  if (hz > 14000.0f) {
    hz = 14000.0f;
  }
  return 1.0f - expf(-SH_TWO_PI * hz / SH_FS);
}

static float sh101_roland_lp(float in, float cutoff_hz, float res_norm) {
  const float g = sh101_one_pole_g(cutoff_hz);
  const float res = res_norm * (0.4f + s_vibe * 0.25f);
  const float fb = s_lp4 * (0.1f + res * 0.65f);
  float x = in - fb;

  s_lp1 += g * (x - s_lp1);
  s_lp2 += g * (s_lp1 - s_lp2);
  s_lp3 += g * (s_lp2 - s_lp3);
  s_lp4 += g * (s_lp3 - s_lp4);
  return s_lp4;
}

static void sh101_update_sustain(void) {
  const float warm = 1.0f - fminf(s_vibe * 1.05f, 1.0f);
  const float hollow = fminf(fmaxf((s_vibe - 0.42f) * 1.75f, 0.0f), 1.0f);
  s_sustain_lvl = 0.64f * warm + 0.58f * hollow + 0.12f;
  if (s_sustain_lvl > 0.92f) {
    s_sustain_lvl = 0.92f;
  }
}

void sh101_bass_reset(void) {
  s_phase = 0U;
  s_phase_sub = 0U;
  s_phase_mod = 0U;
  s_pwm_lfo = 0U;
  s_w_cur = 0.0f;
  s_w_tgt = 0.0f;
  s_env_stage = k_sh101_stage_idle;
  s_gate = 0U;
  s_amp = 0.0f;
  s_filt_env = 0.0f;
  s_fm_env = 0.0f;
  s_vibe = sh101_norm100(30U);
  s_pwm_depth = sh101_norm100(46U);
  s_sub_lvl = sh101_norm100(52U);
  s_hollow = sh101_norm100(28U);
  s_saw_mix = sh101_norm100(18U);
  s_glide = sh101_norm100(24U);
  s_attack_sec = sh101_time_sec(10U, 0.002f, 0.28f);
  s_decay_sec = sh101_time_sec(42U, 0.03f, 1.25f);
  s_release_sec = sh101_time_sec(38U, 0.04f, 1.5f);
  s_lp1 = 0.0f;
  s_lp2 = 0.0f;
  s_lp3 = 0.0f;
  s_lp4 = 0.0f;
  sh101_update_sustain();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  sh101_bass_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  s_gate = 1U;
  s_w_tgt = (float)osc_w0(params->pitch);
  if (s_glide < 0.02f || s_w_cur < 1.0f) {
    s_w_cur = s_w_tgt;
  }
  s_phase = 0U;
  s_phase_sub = 0U;
  s_phase_mod = 0U;
  s_fm_env = 1.0f;
  if (s_env_stage == k_sh101_stage_idle || s_env_stage == k_sh101_stage_release) {
    s_env_stage = k_sh101_stage_attack;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
  if (s_env_stage != k_sh101_stage_idle) {
    s_env_stage = k_sh101_stage_release;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_sh101_param_vibe:
      s_vibe = sh101_norm100(value);
      sh101_update_sustain();
      break;
    case k_sh101_param_pwm:
      s_pwm_depth = sh101_norm100(value);
      break;
    case k_sh101_param_sub:
      s_sub_lvl = sh101_norm100(value);
      break;
    case k_sh101_param_hollow:
      s_hollow = sh101_norm100(value);
      break;
    case k_sh101_param_saw:
      s_saw_mix = sh101_norm100(value);
      break;
    case k_sh101_param_glide:
      s_glide = sh101_norm100(value);
      break;
    case k_sh101_param_attack:
      s_attack_sec = sh101_time_sec(value, 0.002f, 0.28f);
      break;
    case k_sh101_param_decay:
      s_decay_sec = sh101_time_sec(value, 0.03f, 1.25f);
      break;
    case k_sh101_param_release:
      s_release_sec = sh101_time_sec(value, 0.04f, 1.5f);
      break;
    default:
      break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  s_w_tgt = (float)osc_w0(params->pitch);

  const float glide_coef = 0.00012f + s_glide * s_glide * 0.009f;
  const float attack_step =
      (s_attack_sec > 0.0001f) ? (1.0f / (s_attack_sec * SH_FS)) : 1.0f;
  const float decay_step =
      (s_decay_sec > 0.0001f) ? (1.0f / (s_decay_sec * SH_FS)) : 1.0f;
  const float release_step =
      (s_release_sec > 0.0001f) ? (1.0f / (s_release_sec * SH_FS)) : 1.0f;

  const float fm_decay = 0.00012f + s_hollow * 0.0018f;
  const float host_cut = (float)params->cutoff * (1.0f / 8191.0f);
  const float host_res = (float)params->resonance * (1.0f / 8191.0f);
  const uint32_t pwm_lfo_inc = 118496331U;

  const float warm = 1.0f - fminf(s_vibe * 0.9f, 1.0f);
  const float hollow_mix = fminf(fmaxf((s_vibe - 0.38f) * 1.6f, 0.0f), 1.0f);

  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    s_w_cur += (s_w_tgt - s_w_cur) * glide_coef;
    const uint32_t w = sh101_float_to_w(s_w_cur);
    const uint32_t w_sub = w >> 1;
    const uint32_t w_mod = sh101_float_to_w(s_w_cur * (1.0f + hollow_mix * 0.5f));

    s_pwm_lfo += pwm_lfo_inc;
    s_fm_env -= s_fm_env * fm_decay;
    if (s_fm_env < 0.08f) {
      s_fm_env = 0.08f;
    }

    if (s_env_stage == k_sh101_stage_attack) {
      s_amp += attack_step;
      s_filt_env += attack_step * 0.9f;
      if (s_amp >= 1.0f) {
        s_amp = 1.0f;
        s_filt_env = fminf(s_filt_env, 1.0f);
        s_env_stage = k_sh101_stage_decay;
      }
    } else if (s_env_stage == k_sh101_stage_decay) {
      s_amp += (s_sustain_lvl - s_amp) * decay_step;
      s_filt_env += (s_sustain_lvl * 0.5f - s_filt_env) * decay_step;
      if (fabsf(s_amp - s_sustain_lvl) < 0.002f) {
        s_amp = s_sustain_lvl;
        s_env_stage = k_sh101_stage_sustain;
      }
    } else if (s_env_stage == k_sh101_stage_sustain) {
      if (s_gate == 0U) {
        s_env_stage = k_sh101_stage_release;
      }
      s_amp = s_sustain_lvl;
      s_filt_env = s_sustain_lvl * 0.5f;
    } else if (s_env_stage == k_sh101_stage_release) {
      s_amp -= release_step;
      s_filt_env -= release_step * 0.75f;
      if (s_amp < SH_AMP_FLOOR) {
        s_amp = 0.0f;
        s_filt_env = 0.0f;
        s_env_stage = k_sh101_stage_idle;
      }
    }

    float sig = 0.0f;
    if (s_env_stage != k_sh101_stage_idle) {
      const float width =
          0.52f - warm * 0.08f + sh101_lfo(s_pwm_lfo) * s_pwm_depth * 0.14f;
      const float pulse = sh101_pulse(s_phase, width);
      const float sub = sh101_pulse(s_phase_sub, 0.5f);
      const float saw = sh101_saw(s_phase) * s_saw_mix * (0.25f + warm * 0.35f);

      const float fm_idx = s_hollow * s_fm_env * (1.2f + hollow_mix * 2.4f);
      const float mod = sh101_sin(s_phase_mod) * fm_idx;
      float car = (float)s_phase * (1.0f / 4294967296.0f) * SH_TWO_PI;
      car += mod * (0.35f + hollow_mix * 0.45f);
      const float fm_voice = sinf(car) * hollow_mix * 0.55f;

      sig = pulse * (0.62f + warm * 0.28f);
      sig += sub * s_sub_lvl * (0.48f + warm * 0.2f);
      sig += saw;
      sig += fm_voice * (0.4f + s_hollow * 0.5f);

      const float base_hz =
          60.0f + host_cut * (90.0f + warm * 2200.0f + hollow_mix * 3200.0f);
      const float env_hz = base_hz + s_filt_env * (280.0f + warm * 1100.0f);
      const float res_use = host_res * (0.22f + warm * 0.48f + hollow_mix * 0.18f);

      sig = sh101_roland_lp(sig, env_hz, res_use);
      sig *= s_amp * 0.9f;
    }

    s_phase += w;
    s_phase_sub += w_sub;
    s_phase_mod += w_mod;
    *y++ = f32_to_q31(sig);
  }
}
