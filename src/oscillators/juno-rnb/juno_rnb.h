#pragma once

#include <stdint.h>

enum juno_rnb_param_id {
  k_juno_rnb_param_vibe = 0,
  k_juno_rnb_param_saw = 1,
  k_juno_rnb_param_pwm = 2,
  k_juno_rnb_param_chorus = 3,
  k_juno_rnb_param_glide = 4,
  k_juno_rnb_param_attack = 5,
  k_juno_rnb_param_decay = 6,
  k_juno_rnb_param_release = 7,
  k_juno_rnb_param_hpf = 8,
  k_juno_rnb_num_params = 9
};

void juno_rnb_reset(void);
