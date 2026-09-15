/*
 * TR-808 style kick drum oscillator: sine/triangle, ADR, selectable LP filters.
 */
#include "userosc.h"
#include "utils/fixed_math.h"
#include "dsp_utils.h"
#include "kick808.h"

#include <math.h>

#define KICK_FS 48000.0f
#define KICK_TWO_PI 6.28318530718f

enum kick_env_stage {
  k_kick_stage_idle = 0,
  k_kick_stage_attack,
  k_kick_stage_decay,
  k_kick_stage_release
};

static uint32_t s_phase;
static uint8_t s_env_stage;
static float s_amp;
static float s_pitch_mul;

static float s_wave_mix;
static float s_attack_sec;
static float s_decay_sec;
static float s_release_sec;
static float s_filter_cutoff_hz;
static uint8_t s_filter_type;
static float s_pitch_sweep_depth;

static float s_lp_z1;
static float s_lp_z2;

static float kick_param_norm100(uint16_t value) {
  if (value > 100U) {
    value = 100U;
  }
  return (float)value * 0.01f;
}

static float kick_ms_to_sec(uint16_t value) {
  const float norm = kick_param_norm100(value);
  return 0.0005f + norm * norm * 2.5f;
}

static float kick_filter_cutoff_hz(uint16_t value) {
  const float norm = kick_param_norm100(value);
  return 30.0f + norm * norm * 12000.0f;
}

static float kick_pitch_sweep_depth(uint16_t value) {
  return 1.0f + kick_param_norm100(value) * 48.0f;
}

static float kick_one_pole_g(float cutoff_hz) {
  if (cutoff_hz < 20.0f) {
    cutoff_hz = 20.0f;
  }
  const float x = -KICK_TWO_PI * cutoff_hz / KICK_FS;
  float g = 1.0f - expf(x);
  if (g < 0.0001f) {
    g = 0.0001f;
  }
  if (g > 1.0f) {
    g = 1.0f;
  }
  return g;
}

static float kick_osc_sample(uint32_t phase, float wave_mix) {
  const float norm = (float)phase * (1.0f / 4294967296.0f);
  const float angle = KICK_TWO_PI * norm;
  const float sine = sinf(angle);
  const float tri = 1.0f - 4.0f * fabsf(norm - 0.5f);
  return sine + (tri - sine) * wave_mix;
}

static float kick_filter_process(float in) {
  const float g = kick_one_pole_g(s_filter_cutoff_hz);

  if (s_filter_type == 0U) {
    s_lp_z1 += g * (in - s_lp_z1);
    return s_lp_z1;
  }

  if (s_filter_type == 1U) {
    s_lp_z1 += g * (in - s_lp_z1);
    s_lp_z2 += g * (s_lp_z1 - s_lp_z2);
    return s_lp_z2;
  }

  const float res = 0.35f;
  const float fb = s_lp_z2 * res;
  const float x = in - fb;
  s_lp_z1 += g * (x - s_lp_z1);
  s_lp_z2 += g * (s_lp_z1 - s_lp_z2);
  return s_lp_z2;
}

static void kick_trigger(void) {
  s_env_stage = k_kick_stage_attack;
  s_amp = 0.0f;
  s_pitch_mul = s_pitch_sweep_depth;
  s_phase = 0U;
  s_lp_z1 = 0.0f;
  s_lp_z2 = 0.0f;
}

void kick808_reset(void) {
  s_phase = 0U;
  s_env_stage = k_kick_stage_idle;
  s_amp = 0.0f;
  s_pitch_mul = 1.0f;
  s_wave_mix = 0.0f;
  s_attack_sec = 0.001f;
  s_decay_sec = 0.35f;
  s_release_sec = 0.08f;
  s_filter_cutoff_hz = 8000.0f;
  s_filter_type = 1U;
  s_pitch_sweep_depth = 12.0f;
  s_lp_z1 = 0.0f;
  s_lp_z2 = 0.0f;
}

void OSC_INIT(uint32_t platform, uint32_t api) {
  kick808_reset();
}

void OSC_NOTEON(const user_osc_param_t *const params) {
  (void)params;
  kick_trigger();
}

void OSC_NOTEOFF(const user_osc_param_t *const params) {
  (void)params;
  if (s_env_stage == k_kick_stage_attack || s_env_stage == k_kick_stage_decay) {
    s_env_stage = k_kick_stage_release;
  }
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  switch (index) {
    case k_kick808_param_wave:
      s_wave_mix = dsp_param_to_norm(value);
      break;
    case k_kick808_param_attack:
      s_attack_sec = kick_ms_to_sec(value);
      break;
    case k_kick808_param_decay:
      s_decay_sec = kick_ms_to_sec(value);
      break;
    case k_kick808_param_release:
      s_release_sec = kick_ms_to_sec(value);
      break;
    case k_kick808_param_filter_cutoff:
      s_filter_cutoff_hz = kick_filter_cutoff_hz(value);
      break;
    case k_kick808_param_filter_type:
      s_filter_type = (uint8_t)((value * 3U) >> 16);
      if (s_filter_type > 2U) {
        s_filter_type = 2U;
      }
      break;
    case k_kick808_param_pitch_sweep:
      s_pitch_sweep_depth = kick_pitch_sweep_depth(value);
      break;
    default:
      break;
  }
}

void OSC_CYCLE(const user_osc_param_t *const params, int32_t *yn, const uint32_t frames) {
  const float pitch_decay_coef = 1.0f - expf(-1.0f / (s_decay_sec * KICK_FS * 0.35f));
  const float attack_step =
      (s_attack_sec > 0.0001f) ? (1.0f / (s_attack_sec * KICK_FS)) : 1.0f;
  const float decay_step =
      (s_decay_sec > 0.0001f) ? (1.0f / (s_decay_sec * KICK_FS)) : 1.0f;
  const float release_step =
      (s_release_sec > 0.0001f) ? (1.0f / (s_release_sec * KICK_FS)) : 1.0f;

  const uint32_t base_w = osc_w0(params->pitch);
  q31_t *y = (q31_t *)yn;

  for (uint32_t i = 0; i < frames; i++) {
    if (s_env_stage == k_kick_stage_attack) {
      s_amp += attack_step;
      if (s_amp >= 1.0f) {
        s_amp = 1.0f;
        s_env_stage = k_kick_stage_decay;
      }
    } else if (s_env_stage == k_kick_stage_decay) {
      s_amp -= decay_step;
      if (s_amp <= 0.0f) {
        s_amp = 0.0f;
        s_env_stage = k_kick_stage_idle;
      }
    } else if (s_env_stage == k_kick_stage_release) {
      s_amp -= release_step;
      if (s_amp <= 0.0f) {
        s_amp = 0.0f;
        s_env_stage = k_kick_stage_idle;
      }
    }

    if (s_env_stage != k_kick_stage_idle) {
      s_pitch_mul += (1.0f - s_pitch_mul) * pitch_decay_coef;
    }

    float scaled_w = (float)base_w * s_pitch_mul;
    if (scaled_w > 4294967295.0f) {
      scaled_w = 4294967295.0f;
    }
    const uint32_t w_safe = (uint32_t)scaled_w;

    float sig = 0.0f;
    if (s_env_stage != k_kick_stage_idle && s_amp > 0.00001f) {
      sig = kick_osc_sample(s_phase, s_wave_mix);
      sig *= s_amp;
      sig = kick_filter_process(sig);
      s_phase += w_safe;
    }

    *y++ = f32_to_q31(sig);
  }
}
