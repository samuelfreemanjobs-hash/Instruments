#pragma once

#include <stdint.h>

enum juicy_bass_param_id {
  k_juicy_param_vibe = 0,
  k_juicy_param_sub = 1,
  k_juicy_param_trunk = 2,
  k_juicy_param_tape = 3,
  k_juicy_param_mid = 4,
  k_juicy_param_glide = 5,
  k_juicy_param_attack = 6,
  k_juicy_param_decay = 7,
  k_juicy_param_release = 8,
  k_juicy_num_params = 9
};

void juicy_bass_reset(void);
