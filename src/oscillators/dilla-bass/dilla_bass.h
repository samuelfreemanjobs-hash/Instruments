#pragma once

#include <stdint.h>

enum dilla_bass_param_id {
  k_dilla_param_vibe = 0,
  k_dilla_param_glide = 1,
  k_dilla_param_wobble = 2,
  k_dilla_param_warmth = 3,
  k_dilla_param_grit = 4,
  k_dilla_param_attack = 5,
  k_dilla_param_decay = 6,
  k_dilla_param_release = 7,
  k_dilla_param_thick = 8,
  k_dilla_num_params = 9
};

void dilla_bass_reset(void);
