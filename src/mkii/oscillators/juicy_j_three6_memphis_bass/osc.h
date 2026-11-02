#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * mkII port of src/oscillators/memphis-juicy/juicy_bass.cpp
 */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum {
    VIBE = 0U,
    SUB = 1U,
    TRUNK = 2U,
    TAPE = 3U,
    MID = 4U,
    GLIDE = 5U,
    ATTACK = 6U,
    DECAY = 7U,
    RELEASE = 8U};

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
    case SUB:
      sub_lvl_ = norm;
      break;
    case TRUNK:
      trunk_ = norm;
      break;
    case TAPE:
      tape_ = norm;
      break;
    case MID:
      mid_ = norm;
      break;
    case GLIDE:
      glide_ = norm;
      break;
    case ATTACK:
      attack_sec_ = timeSec(norm, 0.001f, 0.12f);
      break;
    case DECAY:
      decay_sec_ = timeSec(norm, 0.05f, 1.5f);
      break;
    case RELEASE:
      release_sec_ = timeSec(norm, 0.05f, 1.4f);
      break;
    default:
      break;
    }
  }

  void init(float *) override final { reset(); }

  void reset() override final {
    phase_ = 0U;
    phase_deep_ = 0U;
    w_cur_ = 0.0f;
    w_tgt_ = 0.0f;
    env_stage_ = 0;
    gate_ = 0U;
    amp_ = 0.0f;
    comp_env_ = 0.0f;
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

  const float glide_coef = 0.00012f + glide_ * glide_ * 0.005f;
  const float attack_step =
      (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * k_fs)) : 1.0f;
  const float decay_step =
      (decay_sec_ > 0.0001f) ? (1.0f / (decay_sec_ * k_fs)) : 1.0f;
  const float release_step =
      (release_sec_ > 0.0001f) ? (1.0f / (release_sec_ * k_fs)) : 1.0f;

  const float host_cut = host_cut_;

  for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
    w_cur_ += (w_tgt_ - w_cur_) * glide_coef;
    const uint32_t w = juicy_float_to_w(w_cur_);
    const uint32_t w_deep = w >> 1;

    if (env_stage_ == 1) {
      amp_ += attack_step;
      comp_env_ += attack_step * 1.1f;
      if (amp_ >= 1.0f) {
        amp_ = 1.0f;
        comp_env_ = fminf(comp_env_, 1.0f);
        env_stage_ = 2;
      }
    } else if (env_stage_ == 2) {
      amp_ += (sustain_lvl_ - amp_) * decay_step;
      comp_env_ += (sustain_lvl_ - comp_env_) * decay_step;
      if (fabsf(amp_ - sustain_lvl_) < 0.002f) {
        amp_ = sustain_lvl_;
        env_stage_ = 3;
      }
    } else if (env_stage_ == 3) {
      if (gate_ == 0U) {
        env_stage_ = 4;
      }
      amp_ = sustain_lvl_;
      comp_env_ = sustain_lvl_;
    } else if (env_stage_ == 4) {
      amp_ -= release_step;
      comp_env_ -= release_step;
      if (amp_ < k_amp_floor) {
        amp_ = 0.0f;
        comp_env_ = 0.0f;
        env_stage_ = 0;
      }
    }

    float sig = 0.0f;
    if (env_stage_ != 0) {
      const float sine = juicy_sin(phase_);
      const float deep = juicy_sin(phase_deep_);

      float sub = sine * sub_lvl_;
      sub += deep * trunk_ * (0.55f + sub_lvl_ * 0.25f);
      sub = juicy_sub_lp(sub);

      float mid = juicy_mid_band(sine * comp_env_);
      sig = sub + mid;
      sig = juicy_tape_compress(sig);

      const float cut = 0.92f + host_cut * 0.08f;
      sig *= cut;
      sig *= amp_ * (0.88f + trunk_ * 0.08f);
    }

    phase_ += w;
    phase_deep_ += w_deep;
    out[0] = sig;
  }
  }

private:
  float w0_;
  float w_tgt_;
  float host_cut_;
  float host_res_;

  uint32_t phase_;
  uint32_t phase_deep_;
  float w_cur_;
  uint8_t env_stage_;
  uint8_t gate_;
  float amp_;
  float comp_env_;
  float vibe_;
  float sub_lvl_;
  float trunk_;
  float tape_;
  float mid_;
  float glide_;
  float attack_sec_;
  float decay_sec_;
  float release_sec_;
  float sustain_lvl_;
  float sub_lp_;
  float mid_hp_z_;
  float tape_z_;

  float juicy_norm100(uint16_t value) {
    if (value > 100U) {
      value = 100U;
    }
    return (float)value * 0.01f;
  }

  float juicy_time_sec(uint16_t value, float min_s, float max_s) {
    const float n = juicy_norm100(value);
    return min_s + n * n * max_s;
  }

  uint32_t juicy_float_to_w(float w) {
    if (w < 0.0f) {
      w = 0.0f;
    }
    if (w > 4294967295.0f) {
      w = 4294967295.0f;
    }
    return (uint32_t)w;
  }

  float juicy_sin(uint32_t phase) {
    const float norm = (float)phase * (1.0f / 4294967296.0f);
    return sinf(k_two_pi * norm);
  }

  float juicy_one_pole_g(float hz) {
    if (hz < 20.0f) {
      hz = 20.0f;
    }
    if (hz > 12000.0f) {
      hz = 12000.0f;
    }
    return 1.0f - expf(-k_two_pi * hz / k_fs);
  }

  float juicy_sub_lp(float in) {
    const float hz = 42.0f + (1.0f - trunk_) * 55.0f;
    const float g = juicy_one_pole_g(hz);
    sub_lp_ += g * (in - sub_lp_);
    return sub_lp_;
  }

  float juicy_mid_band(float sine) {
    if (mid_ < 0.01f) {
      return 0.0f;
    }
    float shaped = sine;
    const float drive = 1.0f + mid_ * (2.5f + vibe_ * 2.0f);
    shaped = tanhf(shaped * drive);
    shaped = shaped * shaped * (mid_ * 0.35f);
    const float g = juicy_one_pole_g(220.0f + mid_ * 480.0f);
    mid_hp_z_ += g * (shaped - mid_hp_z_);
    return mid_hp_z_ * mid_ * (0.22f + vibe_ * 0.18f);
  }

  float juicy_tape_compress(float in) {
    if (tape_ < 0.01f) {
      return in;
    }
    const float thresh = 0.55f - tape_ * 0.2f;
    float x = in;
    if (x > thresh) {
      x = thresh + (x - thresh) / (1.0f + tape_ * 4.0f);
    }
    if (x < -thresh) {
      x = -thresh + (x + thresh) / (1.0f + tape_ * 4.0f);
    }
    tape_z_ += 0.0012f * (tanhf(x * (1.0f + tape_ * 1.8f)) - tape_z_);
    return in * (1.0f - tape_ * 0.25f) + tape_z_ * (tape_ * 0.65f);
  }

  void updateSustain() {
    sustain_lvl_ = 0.82f + vibe_ * 0.08f;
    if (sustain_lvl_ > 0.95f) {
      sustain_lvl_ = 0.95f;
    }
  }
};
