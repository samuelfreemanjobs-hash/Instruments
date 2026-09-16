/*
 * Moog Voyager / Model D + SE-1 fat multi-osc bass with gliding squelch filter.
 */
#include "userosc.h"
#include "utils/fixed_math.h"
#include "dsp_utils.h"
#include "voyager_se.h"

#include <math.h>

#define VS_FS 48000.0f
#define VS_TWO_PI 6.28318530718f
#define VS_AMP_FLOOR 0.00008f
#define VS_NUM_OSC 3U

enum voyager_env_stage {
  k_voyager_stage_idle = 0,
  k_voyager_stage_attack,
  k_voyager_stage_decay,
  k_voyager_stage_sustain,
  k_voyager_stage_release
};

static uint32_t s_phase[VS_NUM_OSC];
static uint32_t s_phase_sub;

static float s_w_cur;
static float s_w_tgt;

static uint8_t s_env_stage;
static uint8_t s_gate;
static float s_amp;
static float s_filt_env;

static float s_vibe;
static float s_fat;
static float s_square;
static float s_sub_lvl;
static float s_drive;
static float s_glide;
static float s_squelch;
static float s_attack_sec;
static float s_decay_sec;
static float s_sustain_lvl;

static float s_lp1;
static float s_lp2;
static float s_lp3;
static float s_lp4;

static float voyager_norm100(uint16_t value) {
  if (value > 100U) {
    value = 100U;
  }
  return (float)value * 0.01f;
}

static float voyager_time_sec(uint16_t value, float min_s, float max_s) {
  const float n = voyager_norm100(value);
  return min_s + n * n * max_s;
}

static uint32_t voyager_float_to_w(float w) {
  if (w < 0.0f) {
    w = 0.0f;
  }
  if (w > 4294967295.0f) {
    w = 4294967295.0f;
  }
  return (uint32_t)w;
}

static float voyager_saw(uint32_t phase) {
  return (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
}

static float voyager_square(uint32_t phase) {
  return (phase & 0x80000000U) ? 1.0f : -1.0f;
}

static float voyager_one_pole_g(float hz) {
  if (hz < 40.0f) {
    hz = 40.0f;
  }
  if (hz > 18000.0f) {
    hz = 18000.0f;
  }
  return 1.0f - expf(-VS_TWO_PI * hz / VS_FS);
}

static float voyager_moog_drive(float in) {
  const float amount = s_drive * (0.4f + s_vibe * 0.55f);
  if (amount < 0.01f) {
    return in;
  }
  const float g = 1.0f + amount * 4.5f;
  return tanhf(in * g) * (1.0f / (1.0f + amount * 0.28f));
}

static float voyager_ladder_squelch(float in, float cutoff_hz, float res_norm) {
  const float g = voyager_one_pole_g(cutoff_hz);
  const float sq = s_squelch * (0.55f + s_vibe * 0.45f);
  float res = res_norm + sq * 0.55f;
  if (res > 1.15f) {
    res = 1.15f;
  }
  const float fb = s_lp4 * (0.18f + res * 1.05f);
  float x = in - fb;

  s_lp1 += g * (x - s_lp1);
  s_lp2 += g * (s_lp1 - s_lp2);
  s_lp3 += g * (s_lp2 - s_lp3);
  s_lp4 += g * (s_lp3 - s_lp4);
  return s_lp4 * (1.0f + sq * 0.12f);
}

static void voyager_update_sustain(void) {
  const float model_d = 1.0f - fminf(s_vibe * 0.85f, 1.0f);
  const float squelch_w = fminf(fmaxf((s_vibe - 0.45f) * 1.85f, 0.0f), 1.0f);
  s_sustain_lvl = 0.62f * model_d + 0.48f * squelch_w + 0.22f;
  if (s_sustain_lvl > 0.94f) {
    s_sustain_lvl = 0.94f;
  }
}

static float voyager_stack_sample(void) {
  const float spread = s_fat * (0.004f + s_vibe * 0.006f);
  const float weights[VS_NUM_OSC] = {0.38f, 0.34f, 0.28f};
  const float ratios[VS_NUM_OSC] = {1.0f - spread, 1.0f, 1.0f + spread * 1.15f};

  float mix = 0.0f;
  for (uint32_t o = 0; o < VS_NUM_OSC; o++) {
    const uint32_t w_o = voyager_float_to_w(s_w_cur * ratios[o]);
    const float saw = voyager_saw(s_phase[o]);
    const float sq = voyager_square(s_phase[o]);
    mix += (saw * (1.0f - s_square * 0.85f) + sq * (s_square * 0.85f)) * weights[o];
    s_phase[o] += w_o;
  }

  const uint32_t w_sub = voyager_float_to_w(s_w_cur * 0.5f);
  const float sub = voyager_square(s_phase_sub) * s_sub_lvl * 0.5f;
  s_phase_sub += w_sub;

  return mix + sub;
}

void voyager_se_reset(void) {
  for (uint32_t i = 0; i < VS_NUM_OSC; i++) {
    s_phase[i] = 0U;
  }
  s_phase_sub = 0U;
  s_w_cur = 0.0f;
  s_w_tgt = 0.0f;
  s_env_stage = k_voyager_stage_idle;
  s_gate = 0U;
  s_amp = 0.0f;
  s_filt_env = 0.0f;
  s_vibe = voyager_norm100(42U);
  s_fat = voyager_norm100(55U);
  s_square = voyager_norm100(38U);
  s_sub_lvl = voyager_norm100(45U);
  s_drive = voyager_norm100(35U);
  s_glide = voyager_norm100(48U);
  s_squelch = voyager_norm100(52U);
  s_attack_sec = voyager_time_sec(8U, 0.001f, 0.18f);
  s_decay_sec = voyager_time_sec(40U, 0.025f, 1.4f);
  s_lp1 = 0.0f;
  s_lp2 = 0.0f;
  s_lp3 = 0.0f;
  s_lp4 = 0.0f;
  voyager_update_sustain();
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  voyager_se_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  s_gate = 1U;
  s_w_tgt = (float)osc_w0(params->pitch);
  if (s_glide < 0.02f || s_w_cur < 1.0f) {
    s_w_cur = s_w_tgt;
  }
  for (uint32_t i = 0; i < VS_NUM_OSC; i++) {
    s_phase[i] = 0U;
  }
  s_phase_sub = 0U;
  s_filt_env = 0.85f + s_squelch * 0.15f;
  if (s_env_stage == k_voyager_stage_idle || s_env_stage == k_voyager_stage_release) {
    s_env_stage = k_voyager_stage_attack;
  }
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  s_gate = 0U;
  if (s_env_stage != k_voyager_stage_idle) {
    s_env_stage = k_voyager_stage_release;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_voyager_param_vibe:
      s_vibe = voyager_norm100(value);
      voyager_update_sustain();
      break;
    case k_voyager_param_fat:
      s_fat = voyager_norm100(value);
      break;
    case k_voyager_param_square:
      s_square = voyager_norm100(value);
      break;
    case k_voyager_param_sub:
      s_sub_lvl = voyager_norm100(value);
      break;
    case k_voyager_param_drive:
      s_drive = voyager_norm100(value);
      break;
    case k_voyager_param_glide:
      s_glide = voyager_norm100(value);
      break;
    case k_voyager_param_squelch:
      s_squelch = voyager_norm100(value);
      break;
    case k_voyager_param_attack:
      s_attack_sec = voyager_time_sec(value, 0.001f, 0.18f);
      break;
    case k_voyager_param_decay:
      s_decay_sec = voyager_time_sec(value, 0.025f, 1.4f);
      break;
    default:
      break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  s_w_tgt = (float)osc_w0(params->pitch);

  const float glide_coef = 0.0001f + s_glide * s_glide * 0.014f;
  const float attack_step =
      (s_attack_sec > 0.0001f) ? (1.0f / (s_attack_sec * VS_FS)) : 1.0f;
  const float decay_step =
      (s_decay_sec > 0.0001f) ? (1.0f / (s_decay_sec * VS_FS)) : 1.0f;
  const float filt_decay = decay_step * (1.2f + s_squelch * 2.5f);
  const float release_step = filt_decay * 0.85f;

  const float host_cut = (float)params->cutoff * (1.0f / 8191.0f);
  const float host_res = (float)params->resonance * (1.0f / 8191.0f);

  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    s_w_cur += (s_w_tgt - s_w_cur) * glide_coef;

    if (s_env_stage == k_voyager_stage_attack) {
      s_amp += attack_step;
      s_filt_env += attack_step * (1.5f + s_squelch * 1.2f);
      if (s_amp >= 1.0f) {
        s_amp = 1.0f;
        if (s_filt_env > 1.0f) {
          s_filt_env = 1.0f;
        }
        s_env_stage = k_voyager_stage_decay;
      }
    } else if (s_env_stage == k_voyager_stage_decay) {
      s_amp += (s_sustain_lvl - s_amp) * decay_step;
      s_filt_env -= s_filt_env * filt_decay;
      if (s_filt_env < s_sustain_lvl * (0.25f + s_squelch * 0.35f)) {
        s_filt_env = s_sustain_lvl * (0.25f + s_squelch * 0.35f);
      }
      if (fabsf(s_amp - s_sustain_lvl) < 0.002f) {
        s_amp = s_sustain_lvl;
        s_env_stage = k_voyager_stage_sustain;
      }
    } else if (s_env_stage == k_voyager_stage_sustain) {
      if (s_gate == 0U) {
        s_env_stage = k_voyager_stage_release;
      }
      s_amp = s_sustain_lvl;
    } else if (s_env_stage == k_voyager_stage_release) {
      s_amp -= release_step;
      s_filt_env -= release_step * 1.1f;
      if (s_amp < VS_AMP_FLOOR) {
        s_amp = 0.0f;
        s_filt_env = 0.0f;
        s_env_stage = k_voyager_stage_idle;
      }
    }

    float sig = 0.0f;
    if (s_env_stage != k_voyager_stage_idle) {
      sig = voyager_stack_sample();
      sig = voyager_moog_drive(sig);

      const float model_d = 1.0f - fminf(s_vibe * 0.8f, 1.0f);
      const float base_hz =
          55.0f + host_cut * (110.0f + model_d * 2800.0f + s_vibe * 6500.0f);
      const float squelch_hz =
          base_hz + s_filt_env * (600.0f + s_squelch * 6800.0f + s_vibe * 3200.0f);
      const float res_use = host_res * (0.35f + s_squelch * 0.55f);

      sig = voyager_ladder_squelch(sig, squelch_hz, res_use);
      sig *= s_amp;
    }

    *y++ = f32_to_q31(sig);
  }
}
