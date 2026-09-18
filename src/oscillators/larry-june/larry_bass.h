#pragma once

#include <stdint.h>

enum larry_bass_param_id {
  k_larry_param_vibe = 0,
  k_larry_param_pwm = 1,
  k_larry_param_pulse = 2,
  k_larry_param_rubber = 3,
  k_larry_param_glide = 4,
  k_larry_param_space = 5,
  k_larry_param_attack = 6,
  k_larry_param_decay = 7,
  k_larry_param_release = 8,
  k_larry_num_params = 9
};

void larry_bass_reset(void);
