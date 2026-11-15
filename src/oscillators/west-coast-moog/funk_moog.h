#pragma once

#include <stdint.h>

enum funk_moog_param_id {
  k_funk_moog_param_vibe = 0,
  k_funk_moog_param_detune = 1,
  k_funk_moog_param_fm = 2,
  k_funk_moog_param_fold = 3,
  k_funk_moog_param_glide = 4,
  k_funk_moog_param_attack = 5,
  k_funk_moog_param_decay = 6,
  k_funk_moog_param_release = 7,
  k_funk_moog_param_sub = 8,
  k_funk_moog_num_params = 9
};

void funk_moog_reset(void);
