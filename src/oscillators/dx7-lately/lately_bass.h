#pragma once

#include <stdint.h>

enum lately_bass_param_id {
  k_lately_param_vibe = 0,
  k_lately_param_index = 1,
  k_lately_param_bite = 2,
  k_lately_param_ratio = 3,
  k_lately_param_glide = 4,
  k_lately_param_attack = 5,
  k_lately_param_decay = 6,
  k_lately_param_release = 7,
  k_lately_param_tone = 8,
  k_lately_num_params = 9
};

void lately_bass_reset(void);
