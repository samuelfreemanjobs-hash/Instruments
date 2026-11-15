#pragma once

#include <stdint.h>

enum sh101_bass_param_id {
  k_sh101_param_vibe = 0,
  k_sh101_param_pwm = 1,
  k_sh101_param_sub = 2,
  k_sh101_param_hollow = 3,
  k_sh101_param_saw = 4,
  k_sh101_param_glide = 5,
  k_sh101_param_attack = 6,
  k_sh101_param_decay = 7,
  k_sh101_param_release = 8,
  k_sh101_num_params = 9
};

void sh101_bass_reset(void);
