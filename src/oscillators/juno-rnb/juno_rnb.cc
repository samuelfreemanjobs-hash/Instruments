/*
 * Juno-106 inspired multifunction bass for 80s R&B: DCO, PWM, chorus, 24 dB LP.
 */
#include "userosc.h"
#include "utils/fixed_math.h"
#include "dsp_utils.h"
#include "juno_rnb.h"

#include <math.h>

#define JR_FS 48000.0f
#define JR_TWO_PI 6.28318530718f
#define JR_AMP_FLOOR 0.00008f
#define JR_CHORUS_LEN 1536U

enum juno_env_stage {
  k_juno_stage_idle = 0,
  k_juno_stage_attack,
  k_juno_stage_decay,
  k_juno_stage_sustain,
  k_juno_stage_release
};

static uint32_t s_phase;
static uint32_t s_lfo_phase;
static float s_w_cur;
static float s_w_tgt;

static uint8_t s_env_stage;
static uint8_t s_gate;
static float s_amp;
static float s_filt_env;

static float s_vibe;
static float s_saw_mix;
static float s_pwm_depth;
static float s_chorus_amt;
static float s_glide;
static float s_attack_sec;
static float s_decay_sec;
static float s_release_sec;
static float s_hpf_amt;
static float s_sustain_lvl;

static float s_lp1;
static float s_lp2;
static float s_lp3;
static float s_lp4;
static float s_hp_z;

static float s_ch_buf[JR_CHORUS_LEN];
static uint32_t s_ch_wp;

static float juno_norm100(uint16_t value) {
  if (value > 100U) {
    value = 100U;
  }
  return (float)value * 0.01f;
}

static float juno_time_sec(uint16_t value, float min_s, float max_s) {
  const float n = juno_norm100(value);
  return min_s + n * n * max_s;
}

static uint32_t juno_float_to_w(float w) {
  if (w < 0.0f) {
    w = 0.0f;
  }
  if (w > 4294967295.0f) {
    w = 4294967295.0f;
  }
  return (uint32_t)w;
}

static float juno_saw(uint32_t phase) {
  return (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
}

static float juno_pulse(uint32_t phase, float width) {
  if (width < 0.05f) {
    width = 0.05f;
  }
  if (width > 0.95f) {
    width = 0.95f;
  }
  const uint32_t threshold = (uint32_t)(width * 4294967295.0f);
  return (phase < threshold) ? 1.0f : -1.0f;
}

static float juno_lfo_sin(void) {
  const float norm = (float)s_lfo_phase * (1.0f / 4294967296.0f);
  return sinf(JR_TWO_PI * norm);
}

static float juno_one_pole_g(float hz) {
  if (hz < 30.0f) {
    hz = 30.0f;
  }
  if (hz > 18000.0f) {
    hz = 18000.0f;
  }
  return 1.0f - expf(-JR_TWO_PI * hz / JR_FS);
}

static float juno_hpf(float in) {
  if (s_hpf_amt < 0.01f) {
    return in;
  }
  const float cutoff = 25.0f + s_hpf_amt * s_hpf_amt * 320.0f;
  const float g = juno_one_pole_g(cutoff);
  s_hp_z += g * (in - s_hp_z);
  const float hp = in - s_hp_z;
  s_hp_prev_in = in;
  return hp * (0.35f + s_hpf_amt * 0.85f) + in * (1.0f - s_hpf_amt * 0.45f);
}

static float juno_roland_lp(float in, float cutoff_hz, float res_norm) {
  const float g = juno_one_pole_g(cutoff_hz);
  const float fb = s_lp4 * (0.08f + res_norm * 0.72f);
  float x = in - fb;

  s_lp1 += g * (x - s_lp1);
  s_lp2 += g * (s_lp1 - s_lp2);
  s_lp3 += g * (s_lp2 - s_lp3);
  s_lp4 += g * (s_lp3 - s_lp4);
  return s_lp4;
}

static float juno_chorus(float dry) {
  if (s_chorus_amt < 0.01f) {
    return dry;
  }

  const float lfo = juno_lfo_sin();
  const float base = 280.0f + s_chorus_amt * 220.0f;
  const float mod = 40.0f + s_chorus_amt * 90.0f;
  int32_t delay = (int32_t)(base + lfo * mod);
  if (delay < 24) {
    delay = 24;
  }
  if (delay >= (int32_t)JR_CHORUS_LEN) {
    delay = (int32_t)JR_CHORUS_LEN - 1;
  }

  const uint32_t read = (s_ch_wp + JR_CHORUS_LEN - (uint32_t)delay) % JR_CHORUS_LEN;
  const float wet = s_ch_buf[read];
  s_ch_buf[s_ch_wp] = dry;
  s_ch_wp = (s_ch_wp + 1U) % JR_CHORUS_LEN;

  const float mix = s_chorus_amt * (0.28f + s_vibe * 0.22f);
  return dry * (1.0f - mix) + (dry + wet) * 0.5f * mix;
}

static void juno_update_sustain(void) {
  const float ballad = 1.0f - fminf(s_vibe * 1.25f, 1.0f);
  const float bright = fminf(fmaxf((s_vibe - 0.42f) * 1.7f, 0.0f), 1.0f);
  s_sustain_lvl = 0.58f * ballad + 0.82f * (1.0f - ballad) + bright * 0.12f;
  if (s_sustain_lvl > 0.95f) {
    s_sustain_lvl = 0.95f;
  }
}

void juno_rnb_reset(void) {
  s_phase = 0U;
  s_lfo_phase = 0U;
  s_w_cur = 0.0f;
  s_w_tgt = 0.0f;
  s_env_stage = k_juno_stage_idle;
  s_gate = 0U;
  s_amp = 0.0f;
  s_filt_env = 0.0f;
  s_vibe = juno_norm100(34U);
  s_saw_mix = juno_norm100(72U);
  s_pwm_depth = juno_norm100(26U);
  s_chorus_amt = juno_norm100(44U);
  s_glide = juno_norm100(30U);
  s_attack_sec = juno_time_sec(10U, 0.001f, 0.35f);
  s_decay_sec = juno_time_sec(38U, 0.03f, 1.4f);
  s_release_sec = juno_time_sec(36U, 0.04f, 1.8f);
  s_hpf_amt = juno_norm100(14U);
  s_lp1 = 0.0f;
  s_lp2 = 0.0f;
  s_lp3 = 0.0f;
  s_lp4 = 0.0f;
  s_hp_z = 0.0f;
  s_hp_prev_in = 0.0f;
  s_ch_wp = 0U;
  for (uint32_t i = 0; i < JR_CHORUS_LEN; i++) {
    s_ch_buf[i] = 0.0f;
  }
  juno_update_sustain();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  juno_rnb_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  s_gate = 1U;
  s_w_tgt = (float)osc_w0(params->pitch);
  if (s_glide < 0.02f || s_w_cur < 1.0f) {
    s_w_cur = s_w_tgt;
  }
  if (s_env_stage == k_juno_stage_idle || s_env_stage == k_juno_stage_release) {
    s_env_stage = k_juno_stage_attack;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
  if (s_env_stage != k_juno_stage_idle) {
    s_env_stage = k_juno_stage_release;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_juno_rnb_param_vibe:
      s_vibe = juno_norm100(value);
      juno_update_sustain();
      break;
    case k_juno_rnb_param_saw:
      s_saw_mix = juno_norm100(value);
      break;
    case k_juno_rnb_param_pwm:
      s_pwm_depth = juno_norm100(value);
      break;
    case k_juno_rnb_param_chorus:
      s_chorus_amt = juno_norm100(value);
      break;
    case k_juno_rnb_param_glide:
      s_glide = juno_norm100(value);
      break;
    case k_juno_rnb_param_attack:
      s_attack_sec = juno_time_sec(value, 0.001f, 0.35f);
      break;
    case k_juno_rnb_param_decay:
      s_decay_sec = juno_time_sec(value, 0.03f, 1.4f);
      break;
    case k_juno_rnb_param_release:
      s_release_sec = juno_time_sec(value, 0.04f, 1.8f);
      break;
    case k_juno_rnb_param_hpf:
      s_hpf_amt = juno_norm100(value);
      break;
    default:
      break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  s_w_tgt = (float)osc_w0(params->pitch);

  const float glide_coef = 0.00012f + s_glide * s_glide * 0.01f;
  const float attack_step =
      (s_attack_sec > 0.0001f) ? (1.0f / (s_attack_sec * JR_FS)) : 1.0f;
  const float decay_step =
      (s_decay_sec > 0.0001f) ? (1.0f / (s_decay_sec * JR_FS)) : 1.0f;
  const float release_step =
      (s_release_sec > 0.0001f) ? (1.0f / (s_release_sec * JR_FS)) : 1.0f;

  const float host_cut = (float)params->cutoff * (1.0f / 8191.0f);
  const float host_res = (float)params->resonance * (1.0f / 8191.0f);
  const float lfo_inc = (uint32_t)(osc_w0(0x003C00U) >> 3);

  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    s_w_cur += (s_w_tgt - s_w_cur) * glide_coef;
    const uint32_t w = juno_float_to_w(s_w_cur);

    s_lfo_phase += lfo_inc;

    if (s_env_stage == k_juno_stage_attack) {
      s_amp += attack_step;
      s_filt_env += attack_step * 1.1f;
      if (s_amp >= 1.0f) {
        s_amp = 1.0f;
        s_filt_env = fminf(s_filt_env, 1.0f);
        s_env_stage = k_juno_stage_decay;
      }
    } else if (s_env_stage == k_juno_stage_decay) {
      s_amp += (s_sustain_lvl - s_amp) * decay_step;
      s_filt_env += (s_sustain_lvl * 0.55f - s_filt_env) * decay_step;
      if (fabsf(s_amp - s_sustain_lvl) < 0.002f) {
        s_amp = s_sustain_lvl;
        s_env_stage = k_juno_stage_sustain;
      }
    } else if (s_env_stage == k_juno_stage_sustain) {
      if (s_gate == 0U) {
        s_env_stage = k_juno_stage_release;
      }
      s_amp = s_sustain_lvl;
      s_filt_env = s_sustain_lvl * 0.55f;
    } else if (s_env_stage == k_juno_stage_release) {
      s_amp -= release_step;
      s_filt_env -= release_step * 0.85f;
      if (s_amp < JR_AMP_FLOOR) {
        s_amp = 0.0f;
        s_filt_env = 0.0f;
        s_env_stage = k_juno_stage_idle;
      }
    }

    float sig = 0.0f;
    if (s_env_stage != k_juno_stage_idle) {
      const float pwm = 0.42f + juno_lfo_sin() * s_pwm_depth * 0.22f;
      const float saw = juno_saw(s_phase);
      const float square = juno_pulse(s_phase, pwm);
      sig = saw * s_saw_mix + square * (1.0f - s_saw_mix);

      const float dark = 1.0f - fminf(s_vibe * 1.1f, 1.0f);
      const float base_hz =
          70.0f + host_cut * (120.0f + dark * 420.0f + s_vibe * 3800.0f);
      const float env_hz = base_hz + s_filt_env * (350.0f + s_vibe * 1800.0f);
      const float res_use = host_res * (0.25f + dark * 0.35f);

      sig = juno_hpf(sig);
      sig = juno_roland_lp(sig, env_hz, res_use);
      sig = juno_chorus(sig);
      sig *= s_amp * (0.88f + dark * 0.08f);
    }

    s_phase += w;
    *y++ = f32_to_q31(sig);
  }
}
