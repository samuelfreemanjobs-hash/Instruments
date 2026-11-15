/*
 * Larry June style Bay Area cruise bass: rubbery PWM, JV/XV phatt synth, spacious.
 */
#include "userosc.h"
#include "utils/fixed_math.h"
#include "dsp_utils.h"
#include "larry_bass.h"

#include <math.h>

#define LJ_FS 48000.0f
#define LJ_TWO_PI 6.28318530718f
#define LJ_AMP_FLOOR 0.00008f

enum larry_env_stage {
  k_larry_stage_idle = 0,
  k_larry_stage_attack,
  k_larry_stage_decay,
  k_larry_stage_sustain,
  k_larry_stage_release
};

static uint32_t s_phase_a;
static uint32_t s_phase_b;
static uint32_t s_pwm_lfo;
static uint32_t s_rubber_lfo;

static float s_w_cur;
static float s_w_tgt;

static uint8_t s_env_stage;
static uint8_t s_gate;
static float s_amp;
static float s_filt_track;

static float s_vibe;
static float s_pwm_depth;
static float s_pulse_width;
static float s_rubber;
static float s_glide;
static float s_space;
static float s_attack_sec;
static float s_decay_sec;
static float s_release_sec;
static float s_sustain_lvl;

static float s_lp1;
static float s_lp2;
static float s_hp_z;
static float s_ch_buf[320];
static uint32_t s_ch_wp;

static float larry_norm100(uint16_t value) {
  if (value > 100U) {
    value = 100U;
  }
  return (float)value * 0.01f;
}

static float larry_time_sec(uint16_t value, float min_s, float max_s) {
  const float n = larry_norm100(value);
  return min_s + n * n * max_s;
}

static uint32_t larry_float_to_w(float w) {
  if (w < 0.0f) {
    w = 0.0f;
  }
  if (w > 4294967295.0f) {
    w = 4294967295.0f;
  }
  return (uint32_t)w;
}

static float larry_pulse(uint32_t phase, float width) {
  if (width < 0.12f) {
    width = 0.12f;
  }
  if (width > 0.88f) {
    width = 0.88f;
  }
  const uint32_t th = (uint32_t)(width * 4294967295.0f);
  return (phase < th) ? 1.0f : -1.0f;
}

static float larry_lfo(uint32_t ph) {
  const float norm = (float)ph * (1.0f / 4294967296.0f);
  return sinf(LJ_TWO_PI * norm);
}

static float larry_one_pole_g(float hz) {
  if (hz < 30.0f) {
    hz = 30.0f;
  }
  if (hz > 14000.0f) {
    hz = 14000.0f;
  }
  return 1.0f - expf(-LJ_TWO_PI * hz / LJ_FS);
}

static float larry_space_hpf(float in) {
  const float amt = s_space * (0.45f + s_vibe * 0.25f);
  if (amt < 0.02f) {
    return in;
  }
  const float hz = 35.0f + amt * amt * 220.0f;
  const float g = larry_one_pole_g(hz);
  s_hp_z += g * (in - s_hp_z);
  return in - s_hp_z * amt * 0.85f;
}

static float larry_chorus(float in) {
  const float w = s_space * 0.22f;
  if (w < 0.01f) {
    return in;
  }
  const int32_t d = 22 + (int32_t)(larry_lfo(s_pwm_lfo) * 8.0f);
  const uint32_t read = (s_ch_wp + 320U - (uint32_t)d) % 320U;
  const float wet = s_ch_buf[read];
  s_ch_buf[s_ch_wp] = in;
  s_ch_wp = (s_ch_wp + 1U) % 320U;
  return in * (1.0f - w) + wet * w;
}

static void larry_update_sustain(void) {
  const float cruise = 1.0f - fminf(s_vibe * 0.4f, 0.4f);
  s_sustain_lvl = 0.58f * cruise + 0.72f * (1.0f - cruise) + s_vibe * 0.12f;
  if (s_sustain_lvl > 0.9f) {
    s_sustain_lvl = 0.9f;
  }
}

void larry_bass_reset(void) {
  s_phase_a = 0U;
  s_phase_b = 0U;
  s_pwm_lfo = 0U;
  s_rubber_lfo = 88123U;
  s_w_cur = 0.0f;
  s_w_tgt = 0.0f;
  s_env_stage = k_larry_stage_idle;
  s_gate = 0U;
  s_amp = 0.0f;
  s_filt_track = 0.0f;
  s_vibe = larry_norm100(44U);
  s_pwm_depth = larry_norm100(52U);
  s_pulse_width = larry_norm100(46U);
  s_rubber = larry_norm100(48U);
  s_glide = larry_norm100(50U);
  s_space = larry_norm100(58U);
  s_attack_sec = larry_time_sec(20U, 0.006f, 0.38f);
  s_decay_sec = larry_time_sec(46U, 0.04f, 1.35f);
  s_release_sec = larry_time_sec(50U, 0.05f, 1.7f);
  s_lp1 = 0.0f;
  s_lp2 = 0.0f;
  s_hp_z = 0.0f;
  s_ch_wp = 0U;
  for (uint32_t i = 0; i < 320U; i++) {
    s_ch_buf[i] = 0.0f;
  }
  larry_update_sustain();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  larry_bass_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  s_gate = 1U;
  s_w_tgt = (float)osc_w0(params->pitch);
  if (s_glide < 0.02f || s_w_cur < 1.0f) {
    s_w_cur = s_w_tgt;
  }
  if (s_env_stage == k_larry_stage_idle || s_env_stage == k_larry_stage_release) {
    s_env_stage = k_larry_stage_attack;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
  if (s_env_stage != k_larry_stage_idle) {
    s_env_stage = k_larry_stage_release;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_larry_param_vibe:
      s_vibe = larry_norm100(value);
      larry_update_sustain();
      break;
    case k_larry_param_pwm:
      s_pwm_depth = larry_norm100(value);
      break;
    case k_larry_param_pulse:
      s_pulse_width = larry_norm100(value);
      break;
    case k_larry_param_rubber:
      s_rubber = larry_norm100(value);
      break;
    case k_larry_param_glide:
      s_glide = larry_norm100(value);
      break;
    case k_larry_param_space:
      s_space = larry_norm100(value);
      break;
    case k_larry_param_attack:
      s_attack_sec = larry_time_sec(value, 0.006f, 0.38f);
      break;
    case k_larry_param_decay:
      s_decay_sec = larry_time_sec(value, 0.04f, 1.35f);
      break;
    case k_larry_param_release:
      s_release_sec = larry_time_sec(value, 0.05f, 1.7f);
      break;
    default:
      break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  s_w_tgt = (float)osc_w0(params->pitch);

  const float glide_coef = 0.00007f + s_glide * s_glide * 0.008f;
  const float attack_step =
      (s_attack_sec > 0.0001f) ? (1.0f / (s_attack_sec * LJ_FS)) : 1.0f;
  const float decay_step =
      (s_decay_sec > 0.0001f) ? (1.0f / (s_decay_sec * LJ_FS)) : 1.0f;
  const float release_step =
      (s_release_sec > 0.0001f) ? (1.0f / (s_release_sec * LJ_FS)) : 1.0f;

  const float host_cut = (float)params->cutoff * (1.0f / 8191.0f);
  const float host_res = (float)params->resonance * (1.0f / 8191.0f);
  const uint32_t pwm_inc = 87349631U + (uint32_t)(s_pwm_depth * 1200000.0f);
  const uint32_t rubber_inc = 45249631U;

  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    s_w_cur += (s_w_tgt - s_w_cur) * glide_coef;

    const float rubber_det = 1.0f + larry_lfo(s_rubber_lfo) * s_rubber * 0.005f;
    const uint32_t w_a = larry_float_to_w(s_w_cur);
    const uint32_t w_b = larry_float_to_w(s_w_cur * (1.0028f + s_rubber * 0.004f) * rubber_det);

    s_pwm_lfo += pwm_inc;
    s_rubber_lfo += rubber_inc;

    if (s_env_stage == k_larry_stage_attack) {
      s_amp += attack_step;
      s_filt_track += attack_step * 0.65f;
      if (s_amp >= 1.0f) {
        s_amp = 1.0f;
        s_filt_track = fminf(s_filt_track, 1.0f);
        s_env_stage = k_larry_stage_decay;
      }
    } else if (s_env_stage == k_larry_stage_decay) {
      s_amp += (s_sustain_lvl - s_amp) * decay_step;
      s_filt_track += (s_sustain_lvl * 0.4f - s_filt_track) * decay_step;
      if (fabsf(s_amp - s_sustain_lvl) < 0.002f) {
        s_amp = s_sustain_lvl;
        s_env_stage = k_larry_stage_sustain;
      }
    } else if (s_env_stage == k_larry_stage_sustain) {
      if (s_gate == 0U) {
        s_env_stage = k_larry_stage_release;
      }
      s_amp = s_sustain_lvl;
      s_filt_track = s_sustain_lvl * 0.4f;
    } else if (s_env_stage == k_larry_stage_release) {
      s_amp -= release_step;
      s_filt_track -= release_step * 0.55f;
      if (s_amp < LJ_AMP_FLOOR) {
        s_amp = 0.0f;
        s_filt_track = 0.0f;
        s_env_stage = k_larry_stage_idle;
      }
    }

    float sig = 0.0f;
    if (s_env_stage != k_larry_stage_idle) {
      const float base_w = 0.38f + s_pulse_width * 0.24f;
      const float width =
          base_w + larry_lfo(s_pwm_lfo) * s_pwm_depth * (0.14f + s_vibe * 0.1f);
      const float pulse_a = larry_pulse(s_phase_a, width);
      const float pulse_b = larry_pulse(s_phase_b, 1.0f - width * 0.35f);

      const float phatt = 1.0f - fminf(s_space * 0.35f, 0.35f);
      sig = pulse_a * (0.58f + phatt * 0.15f) + pulse_b * (0.32f + s_rubber * 0.18f);

      const float cruise = 1.0f - fminf(s_vibe * 0.55f, 0.55f);
      float cut_hz = 90.0f + host_cut * (200.0f + cruise * 1800.0f + s_vibe * 2400.0f);
      cut_hz += s_filt_track * (180.0f + s_rubber * 520.0f);
      cut_hz += larry_lfo(s_pwm_lfo) * s_rubber * 120.0f;

      const float g = larry_one_pole_g(cut_hz);
      const float res = host_res * (0.08f + s_rubber * 0.18f);
      const float fb = s_lp2 * res;
      s_lp1 += g * ((sig - fb) - s_lp1);
      s_lp2 += g * (s_lp1 - s_lp2);
      sig = s_lp2;

      sig = larry_space_hpf(sig);
      sig = larry_chorus(sig);
      sig *= s_amp * (0.78f + cruise * 0.12f);
    }

    s_phase_a += w_a;
    s_phase_b += w_b;
    *y++ = f32_to_q31(sig);
  }
}
