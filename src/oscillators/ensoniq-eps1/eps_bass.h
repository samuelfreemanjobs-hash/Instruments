#pragma once

#include <stdint.h>

#define EPS_WT_LEN 64U

enum eps_bass_param_id {
  k_eps_param_vibe = 0,
  k_eps_param_wave = 1,
  k_eps_param_drift = 2,
  k_eps_param_dark = 3,
  k_eps_param_reso = 4,
  k_eps_param_grit = 5,
  k_eps_param_glide = 6,
  k_eps_param_attack = 7,
  k_eps_param_tail = 8,
  k_eps_num_params = 9
};

void eps_bass_reset(void);
