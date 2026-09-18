#pragma once

#include <stdint.h>

enum cardo_bass_param_id {
  k_cardo_param_vibe = 0,
  k_cardo_param_sub = 1,
  k_cardo_param_mid = 2,
  k_cardo_param_velvet = 3,
  k_cardo_param_glide = 4,
  k_cardo_param_glow = 5,
  k_cardo_param_attack = 6,
  k_cardo_param_decay = 7,
  k_cardo_param_release = 8,
  k_cardo_num_params = 9
};

void cardo_bass_reset(void);
