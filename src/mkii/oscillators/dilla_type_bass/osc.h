#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * mkII port of src/oscillators/dilla-bass/dilla_bass.cpp
 */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum {
    VIBE = 0U,
    GLIDE = 1U,
    WOBBLE = 2U,
    WARMTH = 3U,
    GRIT = 4U,
    ATTACK = 5U,
    DECAY = 6U,
    RELEASE = 7U,
    THICK = 8U};

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
    case GLIDE:
      glide_ = norm;
      break;
    case WOBBLE:
      wobble_ = norm;
      break;
    case WARMTH:
      warmth_ = norm;
      break;
    case GRIT:
      grit_ = norm;
      break;
    case ATTACK:
      attack_sec_ = timeSec(norm, 0.002f, 0.28f);
      break;
    case DECAY:
      decay_sec_ = timeSec(norm, 0.04f, 1.2f);
      break;
    case RELEASE:
      release_sec_ = timeSec(norm, 0.05f, 1.5f);
      break;
    case THICK:
      thick_ = norm;
      break;
    default:
      break;
    }
  }

  void init(float *) override final { reset(); }

  void reset() override final {
    phase_ = 0U;
    phase_sub_ = 0U;
    lfo_a_ = 0U;
    lfo_b_ = 881233U;
    rng_ = 0xD11A123U;
    w_cur_ = 0.0f;
    w_tgt_ = 0.0f;
    env_stage_ = 0;
    gate_ = 0U;
    amp_ = 0.0f;
    filt_env_ = 0.0f;
    lp1_ = 0.0f;
    lp2_ = 0.0f;
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

  const float glide_coef = 0.0001f + glide_ * glide_ * 0.011f;
  const float attack_step =
      (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * k_fs)) : 1.0f;
  const float decay_step =
      (decay_sec_ > 0.0001f) ? (1.0f / (decay_sec_ * k_fs)) : 1.0f;
  const float release_step =
      (release_sec_ > 0.0001f) ? (1.0f / (release_sec_ * k_fs)) : 1.0f;

  const float host_cut = host_cut_;
  const float host_res = host_res_;

  const uint32_t lfo_inc_a = 97349631U;
  const uint32_t lfo_inc_b = 147218953U;

  for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
    w_cur_ += (w_tgt_ - w_cur_) * glide_coef;

    lfo_a_ += lfo_inc_a;
    lfo_b_ += lfo_inc_b;

    if ((dilla_rand_u32() & 0x3FFFU) < 18U) {
      const float r = (float)(dilla_rand_u32() & 0xFFFFU) * (1.0f / 32768.0f) - 1.0f;
      drift_tgt_ = r * wobble_ * (0.004f + vibe_ * 0.006f);
    }
    drift_ += (drift_tgt_ - drift_) * 0.00035f;

    float w_f = w_cur_ * (1.0f + drift_);
    w_f += w_cur_ * (dilla_lfo(lfo_a_) * 0.5f + dilla_lfo(lfo_b_) * 0.5f) *
           wobble_ * 0.0035f;
    const uint32_t w = dilla_float_to_w(w_f);
    const uint32_t w_sub = w >> 1;

    if (env_stage_ == 1) {
      amp_ += attack_step;
      filt_env_ += attack_step * 0.75f;
      if (amp_ >= 1.0f) {
        amp_ = 1.0f;
        filt_env_ = fminf(filt_env_, 1.0f);
        env_stage_ = 2;
      }
    } else if (env_stage_ == 2) {
      amp_ += (sustain_lvl_ - amp_) * decay_step;
      filt_env_ += (sustain_lvl_ * 0.45f - filt_env_) * decay_step;
      if (fabsf(amp_ - sustain_lvl_) < 0.002f) {
        amp_ = sustain_lvl_;
        env_stage_ = 3;
      }
    } else if (env_stage_ == 3) {
      if (gate_ == 0U) {
        env_stage_ = 4;
      }
      amp_ = sustain_lvl_;
      filt_env_ = sustain_lvl_ * 0.45f;
    } else if (env_stage_ == 4) {
      amp_ -= release_step;
      filt_env_ -= release_step * 0.7f;
      if (amp_ < k_amp_floor) {
        amp_ = 0.0f;
        filt_env_ = 0.0f;
        env_stage_ = 0;
      }
    }

    float sig = 0.0f;
    if (env_stage_ != 0) {
      const float sine = dilla_sine(phase_);
      const float saw = dilla_saw_soft(phase_);
      const float sub = dilla_sine(phase_sub_);
      const float pillow = 1.0f - fminf(vibe_ * 1.1f, 1.0f);
      const float thick = thick_ * (0.35f + vibe_ * 0.45f);

      sig = sine * (0.72f + pillow * 0.2f);
      sig += saw * thick * (0.25f + vibe_ * 0.35f);
      sig += sub * thick * 0.42f;

      const float base_hz =
          55.0f + host_cut * (80.0f + pillow * 520.0f + vibe_ * 2400.0f);
      const float env_hz = base_hz + filt_env_ * (220.0f + vibe_ * 900.0f);
      const float res_use = host_res * (0.12f + pillow * 0.25f);

      sig = dilla_warm_lp(sig);
      sig = dilla_filter_lp(sig, env_hz, res_use);
      sig = dilla_grit_process(sig);
      sig *= amp_ * (0.9f + pillow * 0.06f);
    }

    phase_ += w;
    phase_sub_ += w_sub;
    out[0] = sig;
  }
  }

private:
  float w0_;
  float w_tgt_;
  float host_cut_;
  float host_res_;

  uint32_t phase_;
  uint32_t phase_sub_;
  uint32_t lfo_a_;
  uint32_t lfo_b_;
  uint32_t rng_;
  float w_cur_;
  float drift_;
  float drift_tgt_;
  uint8_t env_stage_;
  uint8_t gate_;
  float amp_;
  float filt_env_;
  float vibe_;
  float glide_;
  float wobble_;
  float warmth_;
  float grit_;
  float attack_sec_;
  float decay_sec_;
  float release_sec_;
  float thick_;
  float sustain_lvl_;
  float lp1_;
  float lp2_;
  float warm_z_;

  float dilla_norm100(uint16_t value) {
    if (value > 100U) {
      value = 100U;
    }
    return (float)value * 0.01f;
  }

  float dilla_time_sec(uint16_t value, float min_s, float max_s) {
    const float n = dilla_norm100(value);
    return min_s + n * n * max_s;
  }

  uint32_t dilla_float_to_w(float w) {
    if (w < 0.0f) {
      w = 0.0f;
    }
    if (w > 4294967295.0f) {
      w = 4294967295.0f;
    }
    return (uint32_t)w;
  }

  float dilla_saw_soft(uint32_t phase) {
    const float s = (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
    return tanhf(s * 0.85f);
  }

  float dilla_sine(uint32_t phase) {
    const float norm = (float)phase * (1.0f / 4294967296.0f);
    return sinf(k_two_pi * norm);
  }

  float dilla_lfo(uint32_t ph) {
    const float norm = (float)ph * (1.0f / 4294967296.0f);
    return sinf(k_two_pi * norm);
  }

  uint32_t dilla_rand_u32(void) {
    rng_ = rng_ * 1664525U + 1013904223U;
    return rng_;
  }

  float dilla_one_pole_g(float hz) {
    if (hz < 25.0f) {
      hz = 25.0f;
    }
    if (hz > 12000.0f) {
      hz = 12000.0f;
    }
    return 1.0f - expf(-k_two_pi * hz / k_fs);
  }

  float dilla_warm_lp(float in) {
    const float hz = 90.0f + (1.0f - warmth_) * 2200.0f + warmth_ * 380.0f;
    const float g = dilla_one_pole_g(hz);
    warm_z_ += g * (in - warm_z_);
    return in * (1.0f - warmth_ * 0.55f) + warm_z_ * (0.45f + warmth_ * 0.5f);
  }

  float dilla_filter_lp(float in, float cutoff_hz, float res_norm) {
    const float g = dilla_one_pole_g(cutoff_hz);
    const float fb = lp2_ * (0.05f + res_norm * 0.45f);
    const float x = in - fb;
    lp1_ += g * (x - lp1_);
    lp2_ += g * (lp1_ - lp2_);
    return lp2_;
  }

  float dilla_grit_process(float in) {
    if (grit_ < 0.01f) {
      return in;
    }
    const float gain = 1.0f + grit_ * 2.8f;
    float x = tanhf(in * gain);
    return x * (1.0f - grit_ * 0.12f);
  }

  void updateSustain() {
    const float pillow = 1.0f - fminf(vibe_ * 1.15f, 1.0f);
    const float gritty = fminf(fmaxf((vibe_ - 0.55f) * 2.0f, 0.0f), 1.0f);
    sustain_lvl_ = 0.62f * pillow + 0.78f * (1.0f - pillow) * 0.5f + gritty * 0.15f;
    if (sustain_lvl_ > 0.92f) {
      sustain_lvl_ = 0.92f;
    }
  }
};
