/*
 * Prophet-5 inspired 70s funk / 80s R&B bass: dual osc, sync, CEM-style LP.
 */
#include "userosc.h"
#include "utils/fixed_math.h"
#include "dsp_utils.h"
#include "prophet_bass.h"

#include <math.h>

#define PR_FS 48000.0f
#define PR_TWO_PI 6.28318530718f
#define PR_AMP_FLOOR 0.00008f

enum prophet_env_stage {
  k_prophet_stage_idle = 0,
  k_prophet_stage_attack,
  k_prophet_stage_decay,
  k_prophet_stage_sustain,
  k_prophet_stage_release
};

static uint32_t s_phase_a;
static uint32_t s_phase_b;
static uint32_t s_lfo_pwm;

static float s_w_cur;
static float s_w_tgt;

static uint8_t s_env_stage;
static uint8_t s_gate;
static float s_amp;
static float s_filt_env;

static float s_vibe;
static float s_osc_mix;
static float s_detune;
static float s_sync_amt;
static float s_pwm_depth;
static float s_glide;
static float s_attack_sec;
static float s_decay_sec;
static float s_release_sec;
static float s_sustain_lvl;
static float s_poly_mod;

static float s_lp1;
static float s_lp2;
static float s_lp3;
static float s_lp4;

static float prophet_norm100(uint16_t value) {
  if (value > 100U) {
    value = 100U;
  }
  return (float)value * 0.01f;
}

static float prophet_time_sec(uint16_t value, float min_s, float max_s) {
  const float n = prophet_norm100(value);
  return min_s + n * n * max_s;
}

static uint32_t prophet_float_to_w(float w) {
  if (w < 0.0f) {
    w = 0.0f;
  }
  if (w > 4294967295.0f) {
    w = 4294967295.0f;
  }
  return (uint32_t)w;
}

static float prophet_saw(uint32_t phase) {
  return (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
}

static float prophet_pulse(uint32_t phase, float width) {
  if (width < 0.06f) {
    width = 0.06f;
  }
  if (width > 0.94f) {
    width = 0.94f;
  }
  const uint32_t th = (uint32_t)(width * 4294967295.0f);
  return (phase < th) ? 1.0f : -1.0f;
}

static float prophet_lfo(uint32_t ph) {
  const float norm = (float)ph * (1.0f / 4294967296.0f);
  return sinf(PR_TWO_PI * norm);
}

static float prophet_one_pole_g(float hz) {
  if (hz < 30.0f) {
    hz = 30.0f;
  }
  if (hz > 16000.0f) {
    hz = 16000.0f;
  }
  return 1.0f - expf(-PR_TWO_PI * hz / PR_FS);
}

static float prophet_cem_lp(float in, float cutoff_hz, float res_norm) {
  const float g = prophet_one_pole_g(cutoff_hz);
  const float res = res_norm * (0.55f + s_vibe * 0.35f);
  const float fb = (s_lp3 + s_lp4 * 0.5f) * (0.08f + res * 0.95f);
  float x = in - fb;

  s_lp1 += g * (x - s_lp1);
  s_lp2 += g * (s_lp1 - s_lp2);
  s_lp3 += g * (s_lp2 - s_lp3);
  s_lp4 += g * (s_lp3 - s_lp4);
  return s_lp4 * (1.0f + res * 0.08f);
}

static void prophet_update_sustain(void) {
  const float funk70 = 1.0f - fminf(fmaxf((s_vibe - 0.38f) * 1.8f, 0.0f), 1.0f);
  const float rb80 = fminf(fmaxf((s_vibe - 0.32f) * 1.45f, 0.0f), 1.0f);
  s_sustain_lvl = 0.68f * funk70 + 0.76f * rb80 + 0.08f;
  if (s_sustain_lvl > 0.93f) {
    s_sustain_lvl = 0.93f;
  }
  s_poly_mod = 0.15f + rb80 * 0.35f + (1.0f - funk70) * 0.12f;
}

void prophet_bass_reset(void) {
  s_phase_a = 0U;
  s_phase_b = 0U;
  s_lfo_pwm = 0U;
  s_w_cur = 0.0f;
  s_w_tgt = 0.0f;
  s_env_stage = k_prophet_stage_idle;
  s_gate = 0U;
  s_amp = 0.0f;
  s_filt_env = 0.0f;
  s_vibe = prophet_norm100(45U);
  s_osc_mix = prophet_norm100(58U);
  s_detune = prophet_norm100(30U);
  s_sync_amt = prophet_norm100(20U);
  s_pwm_depth = prophet_norm100(34U);
  s_glide = prophet_norm100(26U);
  s_attack_sec = prophet_time_sec(10U, 0.001f, 0.32f);
  s_decay_sec = prophet_time_sec(40U, 0.025f, 1.35f);
  s_release_sec = prophet_time_sec(38U, 0.035f, 1.6f);
  s_lp1 = 0.0f;
  s_lp2 = 0.0f;
  s_lp3 = 0.0f;
  s_lp4 = 0.0f;
  prophet_update_sustain();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  prophet_bass_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  s_gate = 1U;
  s_w_tgt = (float)osc_w0(params->pitch);
  if (s_glide < 0.02f || s_w_cur < 1.0f) {
    s_w_cur = s_w_tgt;
  }
  if (s_env_stage == k_prophet_stage_idle || s_env_stage == k_prophet_stage_release) {
    s_env_stage = k_prophet_stage_attack;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
  if (s_env_stage != k_prophet_stage_idle) {
    s_env_stage = k_prophet_stage_release;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_prophet_param_vibe:
      s_vibe = prophet_norm100(value);
      prophet_update_sustain();
      break;
    case k_prophet_param_osc_mix:
      s_osc_mix = prophet_norm100(value);
      break;
    case k_prophet_param_detune:
      s_detune = prophet_norm100(value);
      break;
    case k_prophet_param_sync:
      s_sync_amt = prophet_norm100(value);
      break;
    case k_prophet_param_pwm:
      s_pwm_depth = prophet_norm100(value);
      break;
    case k_prophet_param_glide:
      s_glide = prophet_norm100(value);
      break;
    case k_prophet_param_attack:
      s_attack_sec = prophet_time_sec(value, 0.001f, 0.32f);
      break;
    case k_prophet_param_decay:
      s_decay_sec = prophet_time_sec(value, 0.025f, 1.35f);
      break;
    case k_prophet_param_release:
      s_release_sec = prophet_time_sec(value, 0.035f, 1.6f);
      break;
    default:
      break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  s_w_tgt = (float)osc_w0(params->pitch);

  const float glide_coef = 0.00013f + s_glide * s_glide * 0.0095f;
  const float attack_step =
      (s_attack_sec > 0.0001f) ? (1.0f / (s_attack_sec * PR_FS)) : 1.0f;
  const float decay_step =
      (s_decay_sec > 0.0001f) ? (1.0f / (s_decay_sec * PR_FS)) : 1.0f;
  const float release_step =
      (s_release_sec > 0.0001f) ? (1.0f / (s_release_sec * PR_FS)) : 1.0f;

  const float host_cut = (float)params->cutoff * (1.0f / 8191.0f);
  const float host_res = (float)params->resonance * (1.0f / 8191.0f);
  const uint32_t pwm_lfo_inc = 167496331U;

  const float funk_w = 1.0f - fminf(s_vibe * 0.55f, 1.0f);
  const float rb_w = fminf(fmaxf((s_vibe - 0.4f) * 1.65f, 0.0f), 1.0f);

  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    s_w_cur += (s_w_tgt - s_w_cur) * glide_coef;

    const float det = 1.0f + s_detune * (0.006f + funk_w * 0.008f);
    const uint32_t w_a = prophet_float_to_w(s_w_cur);
    const uint32_t w_b = prophet_float_to_w(s_w_cur * det);

    s_lfo_pwm += pwm_lfo_inc;

    const uint32_t pa_prev = s_phase_a;
    s_phase_a += w_a;
    if (s_phase_a < pa_prev && s_sync_amt > 0.01f) {
      const float reset = s_sync_amt * (0.35f + funk_w * 0.65f);
      s_phase_b = (uint32_t)((float)s_phase_a * reset);
    } else {
      s_phase_b += w_b;
    }

    if (s_env_stage == k_prophet_stage_attack) {
      s_amp += attack_step;
      s_filt_env += attack_step * (1.05f + funk_w * 0.35f);
      if (s_amp >= 1.0f) {
        s_amp = 1.0f;
        s_filt_env = fminf(s_filt_env, 1.0f);
        s_env_stage = k_prophet_stage_decay;
      }
    } else if (s_env_stage == k_prophet_stage_decay) {
      s_amp += (s_sustain_lvl - s_amp) * decay_step;
      s_filt_env += (s_sustain_lvl * 0.6f - s_filt_env) * decay_step;
      if (fabsf(s_amp - s_sustain_lvl) < 0.002f) {
        s_amp = s_sustain_lvl;
        s_env_stage = k_prophet_stage_sustain;
      }
    } else if (s_env_stage == k_prophet_stage_sustain) {
      if (s_gate == 0U) {
        s_env_stage = k_prophet_stage_release;
      }
      s_amp = s_sustain_lvl;
      s_filt_env = s_sustain_lvl * 0.6f;
    } else if (s_env_stage == k_prophet_stage_release) {
      s_amp -= release_step;
      s_filt_env -= release_step * 0.8f;
      if (s_amp < PR_AMP_FLOOR) {
        s_amp = 0.0f;
        s_filt_env = 0.0f;
        s_env_stage = k_prophet_stage_idle;
      }
    }

    float sig = 0.0f;
    if (s_env_stage != k_prophet_stage_idle) {
      const float width =
          0.45f + s_pwm_depth * 0.22f + prophet_lfo(s_lfo_pwm) * s_pwm_depth * 0.12f;
      const float saw_a = prophet_saw(s_phase_a);
      const float saw_b = prophet_saw(s_phase_b);
      const float pulse_a = prophet_pulse(s_phase_a, width);
      const float pulse_b = prophet_pulse(s_phase_b, 1.0f - width * 0.35f);

      const float saw_mix = saw_a * 0.55f + saw_b * (0.35f + s_detune * 0.2f);
      const float pulse_mix = pulse_a * 0.5f + pulse_b * 0.35f;
      sig = saw_mix * s_osc_mix + pulse_mix * (1.0f - s_osc_mix * 0.85f);

      const float osc_b_tone = saw_b * 0.4f + pulse_b * 0.3f;
      const float base_hz =
          75.0f + host_cut * (140.0f + funk_w * 2800.0f + rb_w * 4200.0f);
      const float poly_hz = osc_b_tone * s_poly_mod * (420.0f + rb_w * 380.0f);
      const float env_hz = base_hz + s_filt_env * (520.0f + funk_w * 1400.0f) + poly_hz;
      const float res_use = host_res * (0.28f + funk_w * 0.42f + rb_w * 0.22f);

      sig = prophet_cem_lp(sig, env_hz, res_use);
      sig *= s_amp * (0.92f + rb_w * 0.05f);
    }

    *y++ = f32_to_q31(sig);
  }
}
