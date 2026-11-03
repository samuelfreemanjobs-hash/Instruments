/*
 * Moog Sub Phatty inspired bass: dual osc, sub, Multidrive, 24 dB ladder LP.
 */
#include "userosc.h"
#include "utils/fixed_math.h"
#include "dsp_utils.h"
#include "sub_phatty.h"

#include <math.h>

#define PH_FS 48000.0f
#define PH_TWO_PI 6.28318530718f
#define PH_AMP_FLOOR 0.00008f

enum phatty_env_stage {
  k_phatty_stage_idle = 0,
  k_phatty_stage_attack,
  k_phatty_stage_decay,
  k_phatty_stage_sustain,
  k_phatty_stage_release
};

static uint32_t s_phase1;
static uint32_t s_phase2;
static uint32_t s_phase_sub;
static uint32_t s_pwm_lfo;

static float s_w_cur;
static float s_w_tgt;

static uint8_t s_env_stage;
static uint8_t s_gate;
static float s_amp;
static float s_filt_env;

static float s_vibe;
static float s_osc_mix;
static float s_detune;
static float s_sub_lvl;
static float s_drive;
static float s_glide;
static float s_attack_sec;
static float s_decay_sec;
static float s_release_sec;
static float s_sustain_lvl;

static float s_lp1;
static float s_lp2;
static float s_lp3;
static float s_lp4;

static float phatty_norm100(uint16_t value) {
  if (value > 100U) {
    value = 100U;
  }
  return (float)value * 0.01f;
}

static float phatty_time_sec(uint16_t value, float min_s, float max_s) {
  const float n = phatty_norm100(value);
  return min_s + n * n * max_s;
}

static uint32_t phatty_float_to_w(float w) {
  if (w < 0.0f) {
    w = 0.0f;
  }
  if (w > 4294967295.0f) {
    w = 4294967295.0f;
  }
  return (uint32_t)w;
}

static float phatty_saw(uint32_t phase) {
  return (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
}

static float phatty_square(uint32_t phase) {
  return (phase & 0x80000000U) ? 1.0f : -1.0f;
}

static float phatty_pulse(uint32_t phase, float width) {
  if (width < 0.08f) {
    width = 0.08f;
  }
  if (width > 0.92f) {
    width = 0.92f;
  }
  const uint32_t th = (uint32_t)(width * 4294967295.0f);
  return (phase < th) ? 1.0f : -1.0f;
}

static float phatty_lfo(uint32_t ph) {
  const float norm = (float)ph * (1.0f / 4294967296.0f);
  return sinf(PH_TWO_PI * norm);
}

static float phatty_one_pole_g(float hz) {
  if (hz < 35.0f) {
    hz = 35.0f;
  }
  if (hz > 18000.0f) {
    hz = 18000.0f;
  }
  return 1.0f - expf(-PH_TWO_PI * hz / PH_FS);
}

static float phatty_multidrive(float in) {
  const float amount = s_drive * (0.35f + s_vibe * 0.75f);
  if (amount < 0.01f) {
    return in;
  }
  const float gain = 1.0f + amount * 7.0f;
  float x = in * gain;
  if (x > 0.0f) {
    x = tanhf(x * (1.0f + amount * 0.4f));
  } else {
    x = tanhf(x * (1.0f + amount * 0.25f)) * (1.0f - amount * 0.08f);
  }
  return x * (1.0f / (1.0f + amount * 0.35f));
}

static float phatty_ladder_lp(float in, float cutoff_hz, float res_norm) {
  const float g = phatty_one_pole_g(cutoff_hz);
  const float k = 3.8f * res_norm;
  const float fb = s_lp4 * (0.12f + res_norm * 0.82f);
  float x = in - fb * k;

  s_lp1 += g * (x - s_lp1);
  s_lp2 += g * (s_lp1 - s_lp2);
  s_lp3 += g * (s_lp2 - s_lp3);
  s_lp4 += g * (s_lp3 - s_lp4);
  return s_lp4;
}

static void phatty_update_sustain(void) {
  const float clean = 1.0f - fminf(s_vibe * 1.1f, 1.0f);
  const float mean = fminf(fmaxf((s_vibe - 0.35f) * 1.5f, 0.0f), 1.0f);
  s_sustain_lvl = 0.55f * clean + 0.82f * mean + fminf(s_vibe * 0.15f, 0.12f);
  if (s_sustain_lvl > 0.94f) {
    s_sustain_lvl = 0.94f;
  }
}

void sub_phatty_reset(void) {
  s_phase1 = 0U;
  s_phase2 = 0U;
  s_phase_sub = 0U;
  s_pwm_lfo = 0U;
  s_w_cur = 0.0f;
  s_w_tgt = 0.0f;
  s_env_stage = k_phatty_stage_idle;
  s_gate = 0U;
  s_amp = 0.0f;
  s_filt_env = 0.0f;
  s_vibe = phatty_norm100(40U);
  s_osc_mix = phatty_norm100(55U);
  s_detune = phatty_norm100(22U);
  s_sub_lvl = phatty_norm100(58U);
  s_drive = phatty_norm100(42U);
  s_glide = phatty_norm100(18U);
  s_attack_sec = phatty_time_sec(8U, 0.001f, 0.22f);
  s_decay_sec = phatty_time_sec(36U, 0.02f, 1.1f);
  s_release_sec = phatty_time_sec(32U, 0.03f, 1.4f);
  s_lp1 = 0.0f;
  s_lp2 = 0.0f;
  s_lp3 = 0.0f;
  s_lp4 = 0.0f;
  phatty_update_sustain();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  sub_phatty_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  s_gate = 1U;
  s_w_tgt = (float)osc_w0(params->pitch);
  if (s_glide < 0.02f || s_w_cur < 1.0f) {
    s_w_cur = s_w_tgt;
  }
  if (s_env_stage == k_phatty_stage_idle || s_env_stage == k_phatty_stage_release) {
    s_env_stage = k_phatty_stage_attack;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
  if (s_env_stage != k_phatty_stage_idle) {
    s_env_stage = k_phatty_stage_release;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_phatty_param_vibe:
      s_vibe = phatty_norm100(value);
      phatty_update_sustain();
      break;
    case k_phatty_param_osc_mix:
      s_osc_mix = phatty_norm100(value);
      break;
    case k_phatty_param_detune:
      s_detune = phatty_norm100(value);
      break;
    case k_phatty_param_sub:
      s_sub_lvl = phatty_norm100(value);
      break;
    case k_phatty_param_drive:
      s_drive = phatty_norm100(value);
      break;
    case k_phatty_param_glide:
      s_glide = phatty_norm100(value);
      break;
    case k_phatty_param_attack:
      s_attack_sec = phatty_time_sec(value, 0.001f, 0.22f);
      break;
    case k_phatty_param_decay:
      s_decay_sec = phatty_time_sec(value, 0.02f, 1.1f);
      break;
    case k_phatty_param_release:
      s_release_sec = phatty_time_sec(value, 0.03f, 1.4f);
      break;
    default:
      break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  s_w_tgt = (float)osc_w0(params->pitch);

  const float glide_coef = 0.00014f + s_glide * s_glide * 0.009f;
  const float attack_step =
      (s_attack_sec > 0.0001f) ? (1.0f / (s_attack_sec * PH_FS)) : 1.0f;
  const float decay_step =
      (s_decay_sec > 0.0001f) ? (1.0f / (s_decay_sec * PH_FS)) : 1.0f;
  const float release_step =
      (s_release_sec > 0.0001f) ? (1.0f / (s_release_sec * PH_FS)) : 1.0f;

  const float host_cut = (float)params->cutoff * (1.0f / 8191.0f);
  const float host_res = (float)params->resonance * (1.0f / 8191.0f);
  const uint32_t pwm_lfo_inc = 193496331U;

  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    s_w_cur += (s_w_tgt - s_w_cur) * glide_coef;
    const float det_ratio = 1.0f + s_detune * 0.011f;
    const uint32_t w1 = phatty_float_to_w(s_w_cur);
    const uint32_t w2 = phatty_float_to_w(s_w_cur * det_ratio);
    const uint32_t w_sub = w1 >> 1;

    s_pwm_lfo += pwm_lfo_inc;

    if (s_env_stage == k_phatty_stage_attack) {
      s_amp += attack_step;
      s_filt_env += attack_step * 1.25f;
      if (s_amp >= 1.0f) {
        s_amp = 1.0f;
        s_filt_env = fminf(s_filt_env, 1.0f);
        s_env_stage = k_phatty_stage_decay;
      }
    } else if (s_env_stage == k_phatty_stage_decay) {
      s_amp += (s_sustain_lvl - s_amp) * decay_step;
      s_filt_env += (s_sustain_lvl * 0.7f - s_filt_env) * decay_step;
      if (fabsf(s_amp - s_sustain_lvl) < 0.002f) {
        s_amp = s_sustain_lvl;
        s_env_stage = k_phatty_stage_sustain;
      }
    } else if (s_env_stage == k_phatty_stage_sustain) {
      if (s_gate == 0U) {
        s_env_stage = k_phatty_stage_release;
      }
      s_amp = s_sustain_lvl;
      s_filt_env = s_sustain_lvl * 0.7f;
    } else if (s_env_stage == k_phatty_stage_release) {
      s_amp -= release_step;
      s_filt_env -= release_step * 0.85f;
      if (s_amp < PH_AMP_FLOOR) {
        s_amp = 0.0f;
        s_filt_env = 0.0f;
        s_env_stage = k_phatty_stage_idle;
      }
    }

    float sig = 0.0f;
    if (s_env_stage != k_phatty_stage_idle) {
      const float pwm = 0.48f + phatty_lfo(s_pwm_lfo) * 0.12f * (0.3f + s_vibe * 0.5f);
      const float osc1 = phatty_saw(s_phase1) * (1.0f - s_osc_mix * 0.45f) +
                         phatty_pulse(s_phase1, pwm) * (s_osc_mix * 0.45f);
      const float osc2 = phatty_saw(s_phase2) * (1.0f - s_osc_mix * 0.35f) +
                         phatty_square(s_phase2) * (s_osc_mix * 0.35f);
      const float sub = phatty_square(s_phase_sub);

      const float clean = 1.0f - fminf(s_vibe * 0.95f, 1.0f);
      sig = osc1 * 0.52f + osc2 * (0.38f + s_detune * 0.15f);
      sig += sub * s_sub_lvl * (0.55f + clean * 0.25f);

      sig = phatty_multidrive(sig);

      const float base_hz =
          65.0f + host_cut * (100.0f + clean * 380.0f + s_vibe * 5200.0f);
      const float env_hz = base_hz + s_filt_env * (480.0f + s_vibe * 2200.0f);
      const float res_use = host_res * (0.35f + s_vibe * 0.45f);

      sig = phatty_ladder_lp(sig, env_hz, res_use);
      sig *= s_amp;
    }

    s_phase1 += w1;
    s_phase2 += w2;
    s_phase_sub += w_sub;
    *y++ = f32_to_q31(sig);
  }
}
