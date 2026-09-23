#pragma once

#include <stdint.h>

/* Compact 6-op voice (native DX7 import target). */
struct Fm6OpVoice {
  uint8_t algorithm; /* 0..31 DX7 algorithms (subset implemented per version) */
  uint8_t feedback[6];
  int8_t level[6];
  uint16_t ratio_coarse[6];
  uint16_t ratio_fine[6];
  uint8_t eg_rate[6];
  uint8_t eg_level[6];
};

enum fm6op_dx7_param_id {
  k_fm6op_param_patch = 0,
  k_fm6op_param_algo = 1,
  k_fm6op_param_level = 2,
  k_fm6op_param_velo = 3,
  k_fm6op_param_bright = 4,
  k_fm6op_param_detune = 5,
  k_fm6op_num_params = 6
};

void fm6op_dx7_reset(void);
