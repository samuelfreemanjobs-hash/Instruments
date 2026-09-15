#pragma once

#include <stdint.h>

enum kick808_param_id {
  k_kick808_param_wave = 0,
  k_kick808_param_attack = 1,
  k_kick808_param_decay = 2,
  k_kick808_param_release = 3,
  k_kick808_param_filter_cutoff = 4,
  k_kick808_param_filter_type = 5,
  k_kick808_param_pitch_sweep = 6,
  k_kick808_num_params = 7
};

void kick808_reset(void);
