#pragma once

#include <stdint.h>

/* Compact 4-op voice (DX7 import target — subset of full DX7 voice). */
struct Fm4OpVoice {
  uint8_t algorithm; /* 0..39 opsix-style subset in v2 */
  uint8_t feedback[4];
  int8_t level[4];       /* 0..99 scaled */
  uint16_t ratio_coarse[4];
  uint16_t ratio_fine[4];
  uint8_t eg_rate[4];    /* simplified single-stage decay proxy */
  uint8_t eg_level[4];
};

enum fm4op_dx7_param_id {
  k_fm4op_param_patch = 0,
  k_fm4op_param_algo = 1,
  k_fm4op_param_level = 2,
  k_fm4op_param_velo = 3,
  k_fm4op_param_bright = 4,
  k_fm4op_param_detune = 5,
  k_fm4op_num_params = 6
};

void fm4op_dx7_reset(void);
