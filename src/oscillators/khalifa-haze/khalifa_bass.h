#pragma once

#include <stdint.h>

enum khalifa_bass_param_id {
  k_khalifa_param_vibe = 0,
  k_khalifa_param_sub = 1,
  k_khalifa_param_haze = 2,
  k_khalifa_param_float = 3,
  k_khalifa_param_warmth = 4,
  k_khalifa_param_glide = 5,
  k_khalifa_param_attack = 6,
  k_khalifa_param_decay = 7,
  k_khalifa_param_release = 8,
  k_khalifa_num_params = 9
};

void khalifa_bass_reset(void);
