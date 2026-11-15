#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * mkII port of src/oscillators/khalifa-haze/khalifa_bass.cpp
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
    HAZE = 2U,
    FLOAT = 3U,
    WARMTH = 4U,
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
    case HAZE:
      haze_ = norm;
      break;
    case FLOAT:
      float_amt_ = norm;
      break;
    case WARMTH:
      warmth_ = norm;
      break;
    case GLIDE:
      glide_ = norm;
      break;
    case ATTACK:
      attack_sec_ = timeSec(norm, 0.012f, 0.65f);
      break;
    case DECAY:
      decay_sec_ = timeSec(norm, 0.06f, 1.8f);
      break;
    case RELEASE:
      release_sec_ = timeSec(norm, 0.08f, 2.4f);
      break;
    default:
      break;
    }
  }

  void init(float *) override final { reset(); }

  void reset() override final {
    phase_ = 0U;
    phase_h1_ = 0U;
    phase_h2_ = 0U;
    phase_deep_ = 0U;
    w_cur_ = 0.0f;
    w_tgt_ = 0.0f;
    env_stage_ = 0;
    gate_ = 0U;
    amp_ = 0.0f;
    lp1_ = 0.0f;
    lp2_ = 0.0f;
    lp3_ = 0.0f;
    warm_z_ = 0.0f;
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

  const float glide_coef = 0.00006f + glide_ * glide_ * 0.0065f;
  const float attack_mul = 1.0f + float_amt_ * 0.85f;
  const float release_mul = 1.0f + float_amt_ * 1.2f;
  const float attack_step =
      (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * attack_mul * k_fs)) : 1.0f;
  const float decay_step =
      (decay_sec_ > 0.0001f) ? (1.0f / (decay_sec_ * k_fs)) : 1.0f;
  const float release_step =
      (release_sec_ > 0.0001f) ? (1.0f / (release_sec_ * release_mul * k_fs)) : 1.0f;

  const float host_cut = host_cut_;
  const float cloud = 1.0f - fminf(vibe_ * 0.25f, 0.25f);

  for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
    w_cur_ += (w_tgt_ - w_cur_) * glide_coef;

    const uint32_t w = khalifa_float_to_w(w_cur_);
    const uint32_t w_h1 = khalifa_float_to_w(w_cur_ * (1.0012f + haze_ * 0.0025f));
    const uint32_t w_h2 = khalifa_float_to_w(w_cur_ * (0.9988f - haze_ * 0.0025f));
    const uint32_t w_deep = w >> 1;

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
      float core = khalifa_sin(phase_);
      core += khalifa_sin(phase_h1_) * haze_ * 0.22f;
      core += khalifa_sin(phase_h2_) * haze_ * 0.2f;
      core += khalifa_sin(phase_deep_) * sub_lvl_ * 0.35f * cloud;

      sig = core * sub_lvl_ * (0.72f + cloud * 0.15f);
      sig = khalifa_warm(sig);

      float cut_hz = 48.0f + host_cut * (60.0f + cloud * 180.0f);
      cut_hz += (1.0f - haze_) * 80.0f;
      cut_hz -= vibe_ * 25.0f;
      if (cut_hz < 35.0f) {
        cut_hz = 35.0f;
      }
      sig = khalifa_cloud_lp(sig, cut_hz);
      sig *= amp_ * (0.82f + float_amt_ * 0.08f);
    }

    phase_ += w;
    phase_h1_ += w_h1;
    phase_h2_ += w_h2;
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
  uint32_t phase_h1_;
  uint32_t phase_h2_;
  uint32_t phase_deep_;
  float w_cur_;
  uint8_t env_stage_;
  uint8_t gate_;
  float amp_;
  float vibe_;
  float sub_lvl_;
  float haze_;
  float float_amt_;
  float warmth_;
  float glide_;
  float attack_sec_;
  float decay_sec_;
  float release_sec_;
  float sustain_lvl_;
  float lp1_;
  float lp2_;
  float lp3_;
  float warm_z_;

  float khalifa_norm100(uint16_t value) {
    if (value > 100U) {
      value = 100U;
    }
    return (float)value * 0.01f;
  }

  float khalifa_time_sec(uint16_t value, float min_s, float max_s) {
    const float n = khalifa_norm100(value);
    return min_s + n * n * max_s;
  }

  uint32_t khalifa_float_to_w(float w) {
    if (w < 0.0f) {
      w = 0.0f;
    }
    if (w > 4294967295.0f) {
      w = 4294967295.0f;
    }
    return (uint32_t)w;
  }

  float khalifa_sin(uint32_t phase) {
    const float norm = (float)phase * (1.0f / 4294967296.0f);
    return sinf(k_two_pi * norm);
  }

  float khalifa_one_pole_g(float hz) {
    if (hz < 18.0f) {
      hz = 18.0f;
    }
    if (hz > 8000.0f) {
      hz = 8000.0f;
    }
    return 1.0f - expf(-k_two_pi * hz / k_fs);
  }

  float khalifa_cloud_lp(float in, float hz) {
    const float g = khalifa_one_pole_g(hz);
    lp1_ += g * (in - lp1_);
    lp2_ += g * (lp1_ - lp2_);
    lp3_ += g * (lp2_ - lp3_);
    return lp3_;
  }

  float khalifa_warm(float in) {
    if (warmth_ < 0.01f) {
      return in;
    }
    const float g = 1.0f + warmth_ * 1.4f;
    warm_z_ += 0.0008f * (tanhf(in * g) - warm_z_);
    return in * (1.0f - warmth_ * 0.35f) + warm_z_ * (warmth_ * 0.55f);
  }

  void updateSustain() {
    sustain_lvl_ = 0.75f + vibe_ * 0.12f;
    if (sustain_lvl_ > 0.92f) {
      sustain_lvl_ = 0.92f;
    }
  }
};
