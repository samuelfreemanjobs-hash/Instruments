#pragma once

#include <stdint.h>

enum sub_phatty_param_id {
  k_phatty_param_vibe = 0,
  k_phatty_param_osc_mix = 1,
  k_phatty_param_detune = 2,
  k_phatty_param_sub = 3,
  k_phatty_param_drive = 4,
  k_phatty_param_glide = 5,
  k_phatty_param_attack = 6,
  k_phatty_param_decay = 7,
  k_phatty_param_release = 8,
  k_phatty_num_params = 9
};

void sub_phatty_reset(void);
