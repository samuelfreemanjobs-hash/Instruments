#pragma once

#include <stdint.h>

enum prophet_bass_param_id {
  k_prophet_param_vibe = 0,
  k_prophet_param_osc_mix = 1,
  k_prophet_param_detune = 2,
  k_prophet_param_sync = 3,
  k_prophet_param_pwm = 4,
  k_prophet_param_glide = 5,
  k_prophet_param_attack = 6,
  k_prophet_param_decay = 7,
  k_prophet_param_release = 8,
  k_prophet_num_params = 9
};

void prophet_bass_reset(void);
