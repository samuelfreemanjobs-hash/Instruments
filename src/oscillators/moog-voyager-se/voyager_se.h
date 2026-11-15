#pragma once

#include <stdint.h>

enum voyager_se_param_id {
  k_voyager_param_vibe = 0,
  k_voyager_param_fat = 1,
  k_voyager_param_square = 2,
  k_voyager_param_sub = 3,
  k_voyager_param_drive = 4,
  k_voyager_param_glide = 5,
  k_voyager_param_squelch = 6,
  k_voyager_param_attack = 7,
  k_voyager_param_decay = 8,
  k_voyager_num_params = 9
};

void voyager_se_reset(void);
