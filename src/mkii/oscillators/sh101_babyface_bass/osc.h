#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * mkII port of src/oscillators/sh101-babyface/sh101_bass.cpp
 */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum {
    VIBE = 0U,
    PWM = 1U,
    SUB = 2U,
    HOLLOW = 3U,
    SAW = 4U,
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
    case PWM:
      pwm_depth_ = norm;
      break;
    case SUB:
      sub_lvl_ = norm;
      break;
    case HOLLOW:
      hollow_ = norm;
      break;
    case SAW:
      saw_mix_ = norm;
      break;
    case GLIDE:
      glide_ = norm;
      break;
    case ATTACK:
      attack_sec_ = timeSec(norm, 0.002f, 0.28f);
      break;
    case DECAY:
      decay_sec_ = timeSec(norm, 0.03f, 1.25f);
      break;
    case RELEASE:
      release_sec_ = timeSec(norm, 0.04f, 1.5f);
      break;
    default:
      break;
    }
  }

  void init(float *) override final { reset(); }

  void reset() override final {
    phase_ = 0U;
    phase_sub_ = 0U;
    phase_mod_ = 0U;
    w_cur_ = 0.0f;
    w_tgt_ = 0.0f;
    env_stage_ = 0;
    gate_ = 0U;
    amp_ = 0.0f;
    filt_env_ = 0.0f;
    lp1_ = 0.0f;
    lp2_ = 0.0f;
    lp3_ = 0.0f;
    lp4_ = 0.0f;
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
    phase_ = 0U;
    phase_sub_ = 0U;
    phase_mod_ = 0U;
    fm_env_ = 1.0f;
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

  const float glide_coef = 0.00012f + glide_ * glide_ * 0.009f;
  const float attack_step =
      (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * k_fs)) : 1.0f;
  const float decay_step =
      (decay_sec_ > 0.0001f) ? (1.0f / (decay_sec_ * k_fs)) : 1.0f;
  const float release_step =
      (release_sec_ > 0.0001f) ? (1.0f / (release_sec_ * k_fs)) : 1.0f;

  const float fm_decay = 0.00012f + hollow_ * 0.0018f;
  const float host_cut = host_cut_;
  const float host_res = host_res_;
  const uint32_t pwm_lfo_inc = 118496331U;

  const float warm = 1.0f - fminf(vibe_ * 0.9f, 1.0f);
  const float hollow_mix = fminf(fmaxf((vibe_ - 0.38f) * 1.6f, 0.0f), 1.0f);

  for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
    w_cur_ += (w_tgt_ - w_cur_) * glide_coef;
    const uint32_t w = sh101_float_to_w(w_cur_);
    const uint32_t w_sub = w >> 1;
    const uint32_t w_mod = sh101_float_to_w(w_cur_ * (1.0f + hollow_mix * 0.5f));

    pwm_lfo_ += pwm_lfo_inc;
    fm_env_ -= fm_env_ * fm_decay;
    if (fm_env_ < 0.08f) {
      fm_env_ = 0.08f;
    }

    if (env_stage_ == 1) {
      amp_ += attack_step;
      filt_env_ += attack_step * 0.9f;
      if (amp_ >= 1.0f) {
        amp_ = 1.0f;
        filt_env_ = fminf(filt_env_, 1.0f);
        env_stage_ = 2;
      }
    } else if (env_stage_ == 2) {
      amp_ += (sustain_lvl_ - amp_) * decay_step;
      filt_env_ += (sustain_lvl_ * 0.5f - filt_env_) * decay_step;
      if (fabsf(amp_ - sustain_lvl_) < 0.002f) {
        amp_ = sustain_lvl_;
        env_stage_ = 3;
      }
    } else if (env_stage_ == 3) {
      if (gate_ == 0U) {
        env_stage_ = 4;
      }
      amp_ = sustain_lvl_;
      filt_env_ = sustain_lvl_ * 0.5f;
    } else if (env_stage_ == 4) {
      amp_ -= release_step;
      filt_env_ -= release_step * 0.75f;
      if (amp_ < k_amp_floor) {
        amp_ = 0.0f;
        filt_env_ = 0.0f;
        env_stage_ = 0;
      }
    }

    float sig = 0.0f;
    if (env_stage_ != 0) {
      const float width =
          0.52f - warm * 0.08f + sh101_lfo(pwm_lfo_) * pwm_depth_ * 0.14f;
      const float pulse = sh101_pulse(phase_, width);
      const float sub = sh101_pulse(phase_sub_, 0.5f);
      const float saw = sh101_saw(phase_) * saw_mix_ * (0.25f + warm * 0.35f);

      const float fm_idx = hollow_ * fm_env_ * (1.2f + hollow_mix * 2.4f);
      const float mod = sh101_sin(phase_mod_) * fm_idx;
      float car = (float)phase_ * (1.0f / 4294967296.0f) * k_two_pi;
      car += mod * (0.35f + hollow_mix * 0.45f);
      const float fm_voice = sinf(car) * hollow_mix * 0.55f;

      sig = pulse * (0.62f + warm * 0.28f);
      sig += sub * sub_lvl_ * (0.48f + warm * 0.2f);
      sig += saw;
      sig += fm_voice * (0.4f + hollow_ * 0.5f);

      const float base_hz =
          60.0f + host_cut * (90.0f + warm * 2200.0f + hollow_mix * 3200.0f);
      const float env_hz = base_hz + filt_env_ * (280.0f + warm * 1100.0f);
      const float res_use = host_res * (0.22f + warm * 0.48f + hollow_mix * 0.18f);

      sig = sh101_roland_lp(sig, env_hz, res_use);
      sig *= amp_ * 0.9f;
    }

    phase_ += w;
    phase_sub_ += w_sub;
    phase_mod_ += w_mod;
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
  uint32_t phase_mod_;
  uint32_t pwm_lfo_;
  float w_cur_;
  uint8_t env_stage_;
  uint8_t gate_;
  float amp_;
  float filt_env_;
  float fm_env_;
  float vibe_;
  float pwm_depth_;
  float sub_lvl_;
  float hollow_;
  float saw_mix_;
  float glide_;
  float attack_sec_;
  float decay_sec_;
  float release_sec_;
  float sustain_lvl_;
  float lp1_;
  float lp2_;
  float lp3_;
  float lp4_;

  float sh101_norm100(uint16_t value) {
    if (value > 100U) {
      value = 100U;
    }
    return (float)value * 0.01f;
  }

  float sh101_time_sec(uint16_t value, float min_s, float max_s) {
    const float n = sh101_norm100(value);
    return min_s + n * n * max_s;
  }

  uint32_t sh101_float_to_w(float w) {
    if (w < 0.0f) {
      w = 0.0f;
    }
    if (w > 4294967295.0f) {
      w = 4294967295.0f;
    }
    return (uint32_t)w;
  }

  float sh101_saw(uint32_t phase) {
    return (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
  }

  float sh101_pulse(uint32_t phase, float width) {
    if (width < 0.1f) {
      width = 0.1f;
    }
    if (width > 0.9f) {
      width = 0.9f;
    }
    const uint32_t th = (uint32_t)(width * 4294967295.0f);
    return (phase < th) ? 1.0f : -1.0f;
  }

  float sh101_sin(uint32_t phase) {
    const float norm = (float)phase * (1.0f / 4294967296.0f);
    return sinf(k_two_pi * norm);
  }

  float sh101_lfo(uint32_t ph) {
    const float norm = (float)ph * (1.0f / 4294967296.0f);
    return sinf(k_two_pi * norm);
  }

  float sh101_one_pole_g(float hz) {
    if (hz < 35.0f) {
      hz = 35.0f;
    }
    if (hz > 14000.0f) {
      hz = 14000.0f;
    }
    return 1.0f - expf(-k_two_pi * hz / k_fs);
  }

  float sh101_roland_lp(float in, float cutoff_hz, float res_norm) {
    const float g = sh101_one_pole_g(cutoff_hz);
    const float res = res_norm * (0.4f + vibe_ * 0.25f);
    const float fb = lp4_ * (0.1f + res * 0.65f);
    float x = in - fb;

    lp1_ += g * (x - lp1_);
    lp2_ += g * (lp1_ - lp2_);
    lp3_ += g * (lp2_ - lp3_);
    lp4_ += g * (lp3_ - lp4_);
    return lp4_;
  }

  void updateSustain() {
    const float warm = 1.0f - fminf(vibe_ * 1.05f, 1.0f);
    const float hollow = fminf(fmaxf((vibe_ - 0.42f) * 1.75f, 0.0f), 1.0f);
    sustain_lvl_ = 0.64f * warm + 0.58f * hollow + 0.12f;
    if (sustain_lvl_ > 0.92f) {
      sustain_lvl_ = 0.92f;
    }
  }
};
