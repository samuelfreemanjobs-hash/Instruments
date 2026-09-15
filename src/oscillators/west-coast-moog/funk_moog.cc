/*
 * Parliament / Minimoog funk bass & lead with West Coast FM and wavefolding.
 */
#include "userosc.h"
#include "utils/fixed_math.h"
#include "dsp_utils.h"
#include "funk_moog.h"

#include <math.h>

#define FM_FS 48000.0f
#define FM_TWO_PI 6.28318530718f
#define FM_AMP_FLOOR 0.00008f

enum funk_env_stage {
  k_funk_stage_idle = 0,
  k_funk_stage_attack,
  k_funk_stage_decay,
  k_funk_stage_sustain,
  k_funk_stage_release
};

static uint32_t s_phase_a;
static uint32_t s_phase_b;
static uint32_t s_phase_sub;
static uint32_t s_phase_fm;
static float s_w_cur;
static float s_w_tgt;

static uint8_t s_env_stage;
static uint8_t s_gate;
static float s_amp;
static float s_filt_env;

static float s_vibe;
static float s_detune;
static float s_fm_depth;
static float s_fold;
static float s_glide;
static float s_attack_sec;
static float s_decay_sec;
static float s_release_sec;
static float s_sub_mix;
static float s_sustain_lvl;

static float s_lp1;
static float s_lp2;

static float funk_norm100(uint16_t value) {
  if (value > 100U) {
    value = 100U;
  }
  return (float)value * 0.01f;
}

static float funk_time_sec(uint16_t value, float min_s, float max_s) {
  const float n = funk_norm100(value);
  return min_s + n * n * max_s;
}

static float funk_w_to_float(uint32_t w) {
  return (float)w;
}

static uint32_t funk_float_to_w(float w) {
  if (w < 0.0f) {
    w = 0.0f;
  }
  if (w > 4294967295.0f) {
    w = 4294967295.0f;
  }
  return (uint32_t)w;
}

static float funk_saw(uint32_t phase) {
  return (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
}

static float funk_pulse(uint32_t phase, float width) {
  const uint32_t threshold = (uint32_t)(width * 4294967295.0f);
  return (phase < threshold) ? 1.0f : -1.0f;
}

static float funk_sin(uint32_t phase) {
  const float norm = (float)phase * (1.0f / 4294967296.0f);
  return sinf(FM_TWO_PI * norm);
}

static float funk_wavefold(float x, float amount) {
  if (amount < 0.001f) {
    return x;
  }
  const float gain = 1.0f + amount * 6.5f;
  x *= gain;
  const float folded = sinf(x * (1.2f + amount * 2.0f));
  return x * (1.0f - amount * 0.55f) + folded * (amount * 0.85f);
}

static float funk_one_pole_g(float hz) {
  if (hz < 40.0f) {
    hz = 40.0f;
  }
  if (hz > 16000.0f) {
    hz = 16000.0f;
  }
  return 1.0f - expf(-FM_TWO_PI * hz / FM_FS);
}

static float funk_moog_lp(float in, float cutoff_hz, float res_norm) {
  const float g = funk_one_pole_g(cutoff_hz);
  const float k = 1.0f + res_norm * 3.2f;
  const float fb = s_lp2 * (0.15f + res_norm * 0.55f);
  const float x = in - fb * k;
  s_lp1 += g * (x - s_lp1);
  s_lp2 += g * (s_lp1 - s_lp2);
  return s_lp2;
}

static void funk_update_sustain_level(void) {
  const float bass = 1.0f - fminf(s_vibe * 1.35f, 1.0f);
  const float lead = fminf(fmaxf((s_vibe - 0.28f) * 1.55f, 0.0f), 1.0f);
  s_sustain_lvl = 0.42f * bass + 0.78f * lead + 0.25f * s_vibe * (1.0f - bass);
  if (s_sustain_lvl > 0.92f) {
    s_sustain_lvl = 0.92f;
  }
}

static float funk_voice_mix(float saw_a, float saw_b, float pulse, float fm_sin) {
  const float west = fminf(fmaxf((s_vibe - 0.52f) * 2.1f, 0.0f), 1.0f);
  const float moog = 1.0f - west * 0.65f;
  const float bass = 1.0f - fminf(s_vibe * 1.2f, 1.0f);

  float core = (saw_a + saw_b) * (0.42f * moog + 0.18f);
  core += pulse * (0.35f * moog + 0.12f * bass);
  core += fm_sin * s_fm_depth * (0.25f + west * 1.4f);
  core += saw_a * s_sub_mix * (0.55f + bass * 0.35f);
  return funk_wavefold(core, s_fold * (0.35f + west * 0.9f));
}

void funk_moog_reset(void) {
  s_phase_a = 0U;
  s_phase_b = 0U;
  s_phase_sub = 0U;
  s_phase_fm = 0U;
  s_w_cur = 0.0f;
  s_w_tgt = 0.0f;
  s_env_stage = k_funk_stage_idle;
  s_gate = 0U;
  s_amp = 0.0f;
  s_filt_env = 0.0f;
  s_vibe = funk_norm100(22U);
  s_detune = funk_norm100(38U);
  s_fm_depth = funk_norm100(18U);
  s_fold = funk_norm100(24U);
  s_glide = funk_norm100(32U);
  s_attack_sec = funk_time_sec(12U, 0.001f, 0.45f);
  s_decay_sec = funk_time_sec(42U, 0.02f, 1.8f);
  s_release_sec = funk_time_sec(35U, 0.03f, 2.2f);
  s_sub_mix = funk_norm100(48U);
  s_lp1 = 0.0f;
  s_lp2 = 0.0f;
  funk_update_sustain_level();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  funk_moog_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  s_gate = 1U;
  s_w_tgt = funk_w_to_float(osc_w0(params->pitch));
  if (s_glide < 0.02f || s_w_cur < 1.0f) {
    s_w_cur = s_w_tgt;
  }
  if (s_env_stage == k_funk_stage_idle || s_env_stage == k_funk_stage_release) {
    s_env_stage = k_funk_stage_attack;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
  if (s_env_stage != k_funk_stage_idle) {
    s_env_stage = k_funk_stage_release;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_funk_moog_param_vibe:
      s_vibe = funk_norm100(value);
      funk_update_sustain_level();
      break;
    case k_funk_moog_param_detune:
      s_detune = funk_norm100(value);
      break;
    case k_funk_moog_param_fm:
      s_fm_depth = funk_norm100(value);
      break;
    case k_funk_moog_param_fold:
      s_fold = funk_norm100(value);
      break;
    case k_funk_moog_param_glide:
      s_glide = funk_norm100(value);
      break;
    case k_funk_moog_param_attack:
      s_attack_sec = funk_time_sec(value, 0.001f, 0.45f);
      break;
    case k_funk_moog_param_decay:
      s_decay_sec = funk_time_sec(value, 0.02f, 1.8f);
      break;
    case k_funk_moog_param_release:
      s_release_sec = funk_time_sec(value, 0.03f, 2.2f);
      break;
    case k_funk_moog_param_sub:
      s_sub_mix = funk_norm100(value);
      break;
    default:
      break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  s_w_tgt = funk_w_to_float(osc_w0(params->pitch));

  const float glide_coef = 0.00015f + s_glide * s_glide * 0.012f;
  const float attack_step =
      (s_attack_sec > 0.0001f) ? (1.0f / (s_attack_sec * FM_FS)) : 1.0f;
  const float decay_step =
      (s_decay_sec > 0.0001f) ? (1.0f / (s_decay_sec * FM_FS)) : 1.0f;
  const float release_step =
      (s_release_sec > 0.0001f) ? (1.0f / (s_release_sec * FM_FS)) : 1.0f;

  const float host_cut = (float)params->cutoff * (1.0f / 8191.0f);
  const float host_res = (float)params->resonance * (1.0f / 8191.0f);
  const float west = fminf(fmaxf((s_vibe - 0.52f) * 2.1f, 0.0f), 1.0f);

  const float detune_ratio = 1.0f + s_detune * (0.004f + west * 0.008f);
  const float pulse_w = 0.22f + (1.0f - s_vibe) * 0.28f;

  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    s_w_cur += (s_w_tgt - s_w_cur) * glide_coef;
    const uint32_t w = funk_float_to_w(s_w_cur);
    const uint32_t w_b = funk_float_to_w(s_w_cur * detune_ratio);
    const uint32_t w_sub = w >> 1;
    const uint32_t w_fm = funk_float_to_w(s_w_cur * (2.0f + west * 2.5f));

    if (s_env_stage == k_funk_stage_attack) {
      s_amp += attack_step;
      s_filt_env += attack_step * 1.35f;
      if (s_amp >= 1.0f) {
        s_amp = 1.0f;
        s_filt_env = fminf(s_filt_env, 1.0f);
        s_env_stage = k_funk_stage_decay;
      }
    } else if (s_env_stage == k_funk_stage_decay) {
      s_amp += (s_sustain_lvl - s_amp) * decay_step;
      s_filt_env += (s_sustain_lvl * 0.65f - s_filt_env) * decay_step;
      if (fabsf(s_amp - s_sustain_lvl) < 0.002f) {
        s_amp = s_sustain_lvl;
        s_env_stage = k_funk_stage_sustain;
      }
    } else if (s_env_stage == k_funk_stage_sustain) {
      if (s_gate == 0U) {
        s_env_stage = k_funk_stage_release;
      }
      s_amp = s_sustain_lvl;
      s_filt_env = s_sustain_lvl * 0.65f;
    } else if (s_env_stage == k_funk_stage_release) {
      s_amp -= release_step;
      s_filt_env -= release_step * 0.9f;
      if (s_amp < FM_AMP_FLOOR) {
        s_amp = 0.0f;
        s_filt_env = 0.0f;
        s_env_stage = k_funk_stage_idle;
      }
    }

    float sig = 0.0f;
    if (s_env_stage != k_funk_stage_idle) {
      const float fm = funk_sin(s_phase_fm) * s_fm_depth * (0.35f + west * 1.1f);
      const uint32_t ph_a = s_phase_a + (uint32_t)(fm * 1200000.0f);
      const float saw_a = funk_saw(ph_a);
      const float saw_b = funk_saw(s_phase_b);
      const float pulse = funk_pulse(s_phase_b, pulse_w);
      const float sub = funk_saw(s_phase_sub);

      sig = funk_voice_mix(saw_a, saw_b, pulse, fm);
      sig += sub * s_sub_mix * 0.45f;

      const float base_hz = 90.0f + host_cut * (220.0f + (1.0f - s_vibe) * 6800.0f);
      const float env_hz = base_hz + s_filt_env * (900.0f + s_vibe * 5200.0f);
      sig = funk_moog_lp(sig, env_hz, host_res * (0.35f + (1.0f - west) * 0.55f));
      sig *= s_amp;
    }

    s_phase_a += w;
    s_phase_b += w_b;
    s_phase_sub += w_sub;
    s_phase_fm += w_fm;

    *y++ = f32_to_q31(sig);
  }
}
