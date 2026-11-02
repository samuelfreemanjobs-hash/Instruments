#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * mkII port of src/oscillators/dx7-lately/lately_bass.cpp
 */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum {
    VIBE = 0U,
    INDEX = 1U,
    BITE = 2U,
    RATIO = 3U,
    GLIDE = 4U,
    ATTACK = 5U,
    DECAY = 6U,
    RELEASE = 7U,
    TONE = 8U};

  static constexpr float k_fs = 48000.0f;
  static constexpr float k_two_pi = 6.28318530718f;
  static constexpr float k_w_scale = 4294967296.0f;
  static constexpr float k_amp_floor = 0.00008f;


  static float timeSec(float norm, float min_s, float max_s) {
    return min_s + norm * norm * max_s;
  }

  void setParameter(uint8_t index, int32_t value) override final {
    const float norm = param_10bit_to_f32(value);
    switch (index) {
    case VIBE:
      vibe_ = norm;
      updateSustain();
      break;
    case INDEX:
      index_ = norm;
      break;
    case BITE:
      bite_ = norm;
      break;
    case RATIO:
      ratio_ = norm;
      break;
    case GLIDE:
      glide_ = norm;
      break;
    case ATTACK:
      attack_sec_ = timeSec(norm, 0.0005f, 0.12f);
      break;
    case DECAY:
      decay_sec_ = timeSec(norm, 0.03f, 1.1f);
      break;
    case RELEASE:
      release_sec_ = timeSec(norm, 0.04f, 1.2f);
      break;
    case TONE:
      tone_ = norm;
      break;
    default:
      break;
    }
  }

  void init(float *) override final { reset(); }

  void reset() override final {
    phase_c_ = 0U;
    phase_m1_ = 0U;
    phase_m2_ = 0U;
    w_cur_ = 0.0f;
    w_tgt_ = 0.0f;
    env_stage_ = 0;
    gate_ = 0U;
    amp_ = 0.0f;
    mod_env_ = 0.0f;
    mod_env2_ = 0.0f;
    lp1_ = 0.0f;
    lp2_ = 0.0f;
    updateSustain();
    w0_ = 0.0f;
    host_cut_ = 1.0f;
    host_res_ = 0.5f;
  }

  void setPitch(float w0) { w0_ = w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void setHostCutoffRes(float cut, float res) {
    host_cut_ = cut;
    host_res_ = res;
  }

  void noteOn(uint8_t note, uint8_t velo) override final {
    (void)note;
    (void)velo;
    gate_ = 1U;
    w_tgt_ = w0_ * k_w_scale;
    if (glide_ < 0.02f || w_cur_ < 1.0f) {
    w_cur_ = w_tgt_;
    }
    phase_c_ = 0U;
    phase_m1_ = 0U;
    phase_m2_ = 0U;
    mod_env_ = 1.0f;
    mod_env2_ = 0.85f;
    if (env_stage_ == 0 || env_stage_ == 4) {
    env_stage_ = 1;
    }
  }

  void noteOff(uint8_t note) override final {
    (void)note;
    gate_ = 0U;
    if (env_stage_ != 0) {
    env_stage_ = 4;
    }
  }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    (void)in;
    w_tgt_ = w0_ * k_w_scale;
w_tgt_ = w0_ * k_w_scale;

  const float glide_coef = 0.00014f + glide_ * glide_ * 0.008f;
  const float attack_step =
      (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * k_fs)) : 1.0f;
  const float decay_step =
      (decay_sec_ > 0.0001f) ? (1.0f / (decay_sec_ * k_fs)) : 1.0f;
  const float release_step =
      (release_sec_ > 0.0001f) ? (1.0f / (release_sec_ * k_fs)) : 1.0f;

  const float bite_coef = 0.00008f + bite_ * bite_ * 0.0045f;
  const float bite2_coef = bite_coef * 0.35f;
  const float host_cut = host_cut_;
  const float ratio_m1 = 1.0f + ratio_ * 1.85f;
  const float ratio_m2 = 2.0f + ratio_ * 2.5f;

  for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
    w_cur_ += (w_tgt_ - w_cur_) * glide_coef;
    const uint32_t w_c = lately_float_to_w(w_cur_);
    const uint32_t w_m1 = lately_float_to_w(w_cur_ * ratio_m1);
    const uint32_t w_m2 = lately_float_to_w(w_cur_ * ratio_m2);

    mod_env_ -= mod_env_ * bite_coef;
    mod_env2_ -= mod_env2_ * bite2_coef;
    if (mod_env_ < 0.02f) {
      mod_env_ = 0.02f;
    }
    if (mod_env2_ < 0.04f) {
      mod_env2_ = 0.04f;
    }

    if (env_stage_ == 1) {
      amp_ += attack_step;
      if (amp_ >= 1.0f) {
        amp_ = 1.0f;
        env_stage_ = 2;
      }
    } else if (env_stage_ == 2) {
      amp_ += (sustain_lvl_ - amp_) * decay_step;
      if (fabsf(amp_ - sustain_lvl_) < 0.002f) {
        amp_ = sustain_lvl_;
        env_stage_ = 3;
      }
    } else if (env_stage_ == 3) {
      if (gate_ == 0U) {
        env_stage_ = 4;
      }
      amp_ = sustain_lvl_;
    } else if (env_stage_ == 4) {
      amp_ -= release_step;
      if (amp_ < k_amp_floor) {
        amp_ = 0.0f;
        env_stage_ = 0;
      }
    }

    float sig = 0.0f;
    if (env_stage_ != 0) {
      const float idx_base = index_ * (2.8f + vibe_ * 2.2f);
      const float idx1 = idx_base * mod_env_;
      const float idx2 = idx_base * 0.42f * mod_env2_;

      const float m1 = lately_sin(phase_m1_);
      const float m2 = lately_sin(phase_m2_);
      const float mod = m1 * idx1 + m2 * idx2;

      float car_phase = (float)phase_c_ * (1.0f / 4294967296.0f) * k_two_pi;
      car_phase += mod * (0.85f + vibe_ * 0.25f);
      sig = sinf(car_phase);

      const float cut_boost = 0.65f + host_cut * 0.5f;
      sig = lately_tone_lp(sig * cut_boost);
      sig = tanhf(sig * (1.05f + vibe_ * 0.15f));
      sig *= amp_ * 0.92f;
    }

    phase_c_ += w_c;
    phase_m1_ += w_m1;
    phase_m2_ += w_m2;
    out[0] = sig;
  }
  }

private:
  float w0_;
  float w_tgt_;
  float host_cut_;
  float host_res_;

  uint32_t phase_c_;
  uint32_t phase_m1_;
  uint32_t phase_m2_;
  float w_cur_;
  uint8_t env_stage_;
  uint8_t gate_;
  float amp_;
  float mod_env_;
  float mod_env2_;
  float vibe_;
  float index_;
  float bite_;
  float ratio_;
  float glide_;
  float attack_sec_;
  float decay_sec_;
  float release_sec_;
  float tone_;
  float sustain_lvl_;
  float lp1_;
  float lp2_;

  float lately_norm100(uint16_t value) {
    if (value > 100U) {
      value = 100U;
    }
    return (float)value * 0.01f;
  }

  float lately_time_sec(uint16_t value, float min_s, float max_s) {
    const float n = lately_norm100(value);
    return min_s + n * n * max_s;
  }

  uint32_t lately_float_to_w(float w) {
    if (w < 0.0f) {
      w = 0.0f;
    }
    if (w > 4294967295.0f) {
      w = 4294967295.0f;
    }
    return (uint32_t)w;
  }

  float lately_sin(uint32_t phase) {
    const float norm = (float)phase * (1.0f / 4294967296.0f);
    return sinf(k_two_pi * norm);
  }

  float lately_one_pole_g(float hz) {
    if (hz < 40.0f) {
      hz = 40.0f;
    }
    if (hz > 14000.0f) {
      hz = 14000.0f;
    }
    return 1.0f - expf(-k_two_pi * hz / k_fs);
  }

  float lately_tone_lp(float in) {
    const float hz = 180.0f + (1.0f - tone_) * 6500.0f + tone_ * 1200.0f;
    const float g = lately_one_pole_g(hz);
    lp1_ += g * (in - lp1_);
    lp2_ += g * (lp1_ - lp2_);
    return lp2_;
  }

  void updateSustain() {
    sustain_lvl_ = 0.48f + vibe_ * 0.38f;
    if (sustain_lvl_ > 0.9f) {
      sustain_lvl_ = 0.9f;
    }
  }
};
