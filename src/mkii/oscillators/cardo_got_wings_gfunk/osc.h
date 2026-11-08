#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * mkII port of src/oscillators/cardo-gfunk/cardo_bass.cpp
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
    MID = 2U,
    VELVET = 3U,
    GLIDE = 4U,
    GLOW = 5U,
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
    case MID:
      mid_lvl_ = norm;
      break;
    case VELVET:
      velvet_ = norm;
      break;
    case GLIDE:
      glide_ = norm;
      break;
    case GLOW:
      glow_ = norm;
      break;
    case ATTACK:
      attack_sec_ = timeSec(norm, 0.004f, 0.45f);
      break;
    case DECAY:
      decay_sec_ = timeSec(norm, 0.05f, 1.6f);
      break;
    case RELEASE:
      release_sec_ = timeSec(norm, 0.06f, 2.0f);
      break;
    default:
      break;
    }
  }

  void init(float *) override final { reset(); }

  void reset() override final {
    phase_sub_ = 0U;
    phase_mid_ = 0U;
    phase_mid_det_ = 0U;
    w_cur_ = 0.0f;
    w_tgt_ = 0.0f;
    env_stage_ = 0;
    gate_ = 0U;
    amp_ = 0.0f;
    ch_wp_ = 0U;
    for (uint32_t i = 0; i < 384U; i++) {
    ch_buf_[i] = 0.0f;
    }
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

  const float glide_coef = 0.00008f + glide_ * glide_ * 0.007f;
  const float attack_step =
      (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * k_fs)) : 1.0f;
  const float decay_step =
      (decay_sec_ > 0.0001f) ? (1.0f / (decay_sec_ * k_fs)) : 1.0f;
  const float release_step =
      (release_sec_ > 0.0001f) ? (1.0f / (release_sec_ * k_fs)) : 1.0f;

  const float host_cut = host_cut_;
  const float night = 1.0f - fminf(vibe_ * 0.35f, 0.35f);

  for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
    w_cur_ += (w_tgt_ - w_cur_) * glide_coef;

    const uint32_t w_mid = cardo_float_to_w(w_cur_);
    const uint32_t w_sub = w_mid >> 1;
    const uint32_t w_det = cardo_float_to_w(w_cur_ * (1.0035f + vibe_ * 0.002f));

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
      float sub = cardo_sin(phase_sub_);
      const float g_sub = cardo_one_pole_g(58.0f + night * 40.0f);
      sub_lp_ += g_sub * (sub - sub_lp_);
      sub = sub_lp_ * sub_lvl_ * (0.85f + vibe_ * 0.2f);

      float mid = cardo_soft_saw(phase_mid_) * 0.55f;
      mid += cardo_soft_saw(phase_mid_det_) * 0.45f;
      mid += cardo_sin(phase_mid_) * 0.35f;

      const float mid_cut =
          120.0f + host_cut * (280.0f + velvet_ * 900.0f) + (1.0f - velvet_) * 400.0f;
      const float g_mid = cardo_one_pole_g(mid_cut);
      mid_lp1_ += g_mid * (mid - mid_lp1_);
      mid_lp2_ += g_mid * (mid_lp1_ - mid_lp2_);
      mid = mid_lp2_ * mid_lvl_ * (0.5f + vibe_ * 0.35f);

      sig = sub + mid;
      sig = cardo_velvet_chorus(sig);
      sig = cardo_glow(sig);

      const float master_cut = 70.0f + host_cut * (180.0f + velvet_ * 2200.0f);
      const float g_m = cardo_one_pole_g(master_cut);
      chorus_z_ += g_m * (sig - chorus_z_);
      sig = chorus_z_;

      sig *= amp_ * 0.92f;
    }

    phase_sub_ += w_sub;
    phase_mid_ += w_mid;
    phase_mid_det_ += w_det;
    out[0] = sig;
  }
  }

private:
  float w0_;
  float w_tgt_;
  float host_cut_;
  float host_res_;

  uint32_t phase_sub_;
  uint32_t phase_mid_;
  uint32_t phase_mid_det_;
  float w_cur_;
  uint8_t env_stage_;
  uint8_t gate_;
  float amp_;
  float vibe_;
  float sub_lvl_;
  float mid_lvl_;
  float velvet_;
  float glide_;
  float glow_;
  float attack_sec_;
  float decay_sec_;
  float release_sec_;
  float sustain_lvl_;
  float sub_lp_;
  float mid_lp1_;
  float mid_lp2_;
  float chorus_z_;
  uint32_t ch_wp_;
  float ch_buf_[384];

  float cardo_norm100(uint16_t value) {
    if (value > 100U) {
      value = 100U;
    }
    return (float)value * 0.01f;
  }

  float cardo_time_sec(uint16_t value, float min_s, float max_s) {
    const float n = cardo_norm100(value);
    return min_s + n * n * max_s;
  }

  uint32_t cardo_float_to_w(float w) {
    if (w < 0.0f) {
      w = 0.0f;
    }
    if (w > 4294967295.0f) {
      w = 4294967295.0f;
    }
    return (uint32_t)w;
  }

  float cardo_sin(uint32_t phase) {
    const float norm = (float)phase * (1.0f / 4294967296.0f);
    return sinf(k_two_pi * norm);
  }

  float cardo_soft_saw(uint32_t phase) {
    const float s = (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
    return tanhf(s * 0.55f);
  }

  float cardo_one_pole_g(float hz) {
    if (hz < 25.0f) {
      hz = 25.0f;
    }
    if (hz > 12000.0f) {
      hz = 12000.0f;
    }
    return 1.0f - expf(-k_two_pi * hz / k_fs);
  }

  float cardo_glow(float in) {
    if (glow_ < 0.01f) {
      return in;
    }
    const float g = 1.0f + glow_ * 1.8f;
    return tanhf(in * g) * (1.0f / (1.0f + glow_ * 0.22f));
  }

  float cardo_velvet_chorus(float in) {
    const float depth = velvet_ * 0.35f;
    if (depth < 0.02f) {
      return in;
    }
    const int32_t delay = 28 + (int32_t)(depth * 40.0f);
    const uint32_t read = (ch_wp_ + 384U - (uint32_t)delay) % 384U;
    const float wet = ch_buf_[read];
    ch_buf_[ch_wp_] = in;
    ch_wp_ = (ch_wp_ + 1U) % 384U;
    return in * (1.0f - depth * 0.25f) + wet * depth * 0.25f;
  }

  void updateSustain() {
    sustain_lvl_ = 0.72f + vibe_ * 0.18f;
    if (sustain_lvl_ > 0.94f) {
      sustain_lvl_ = 0.94f;
    }
  }
};
