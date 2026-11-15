#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * mkII port of src/oscillators/sub-phatty/sub_phatty.cpp
 */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum {
    VIBE = 0U,
    OSC_MIX = 1U,
    DETUNE = 2U,
    SUB = 3U,
    DRIVE = 4U,
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
    case OSC_MIX:
      osc_mix_ = norm;
      break;
    case DETUNE:
      detune_ = norm;
      break;
    case SUB:
      sub_lvl_ = norm;
      break;
    case DRIVE:
      drive_ = norm;
      break;
    case GLIDE:
      glide_ = norm;
      break;
    case ATTACK:
      attack_sec_ = timeSec(norm, 0.001f, 0.22f);
      break;
    case DECAY:
      decay_sec_ = timeSec(norm, 0.02f, 1.1f);
      break;
    case RELEASE:
      release_sec_ = timeSec(norm, 0.03f, 1.4f);
      break;
    default:
      break;
    }
  }

  void init(float *) override final { reset(); }

  void reset() override final {
    phase1_ = 0U;
    phase2_ = 0U;
    phase_sub_ = 0U;
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

  const float glide_coef = 0.00014f + glide_ * glide_ * 0.009f;
  const float attack_step =
      (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * k_fs)) : 1.0f;
  const float decay_step =
      (decay_sec_ > 0.0001f) ? (1.0f / (decay_sec_ * k_fs)) : 1.0f;
  const float release_step =
      (release_sec_ > 0.0001f) ? (1.0f / (release_sec_ * k_fs)) : 1.0f;

  const float host_cut = host_cut_;
  const float host_res = host_res_;
  const uint32_t pwm_lfo_inc = 193496331U;

  for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
    w_cur_ += (w_tgt_ - w_cur_) * glide_coef;
    const float det_ratio = 1.0f + detune_ * 0.011f;
    const uint32_t w1 = phatty_float_to_w(w_cur_);
    const uint32_t w2 = phatty_float_to_w(w_cur_ * det_ratio);
    const uint32_t w_sub = w1 >> 1;

    pwm_lfo_ += pwm_lfo_inc;

    if (env_stage_ == 1) {
      amp_ += attack_step;
      filt_env_ += attack_step * 1.25f;
      if (amp_ >= 1.0f) {
        amp_ = 1.0f;
        filt_env_ = fminf(filt_env_, 1.0f);
        env_stage_ = 2;
      }
    } else if (env_stage_ == 2) {
      amp_ += (sustain_lvl_ - amp_) * decay_step;
      filt_env_ += (sustain_lvl_ * 0.7f - filt_env_) * decay_step;
      if (fabsf(amp_ - sustain_lvl_) < 0.002f) {
        amp_ = sustain_lvl_;
        env_stage_ = 3;
      }
    } else if (env_stage_ == 3) {
      if (gate_ == 0U) {
        env_stage_ = 4;
      }
      amp_ = sustain_lvl_;
      filt_env_ = sustain_lvl_ * 0.7f;
    } else if (env_stage_ == 4) {
      amp_ -= release_step;
      filt_env_ -= release_step * 0.85f;
      if (amp_ < k_amp_floor) {
        amp_ = 0.0f;
        filt_env_ = 0.0f;
        env_stage_ = 0;
      }
    }

    float sig = 0.0f;
    if (env_stage_ != 0) {
      const float pwm = 0.48f + phatty_lfo(pwm_lfo_) * 0.12f * (0.3f + vibe_ * 0.5f);
      const float osc1 = phatty_saw(phase1_) * (1.0f - osc_mix_ * 0.45f) +
                         phatty_pulse(phase1_, pwm) * (osc_mix_ * 0.45f);
      const float osc2 = phatty_saw(phase2_) * (1.0f - osc_mix_ * 0.35f) +
                         phatty_square(phase2_) * (osc_mix_ * 0.35f);
      const float sub = phatty_square(phase_sub_);

      const float clean = 1.0f - fminf(vibe_ * 0.95f, 1.0f);
      sig = osc1 * 0.52f + osc2 * (0.38f + detune_ * 0.15f);
      sig += sub * sub_lvl_ * (0.55f + clean * 0.25f);

      sig = phatty_multidrive(sig);

      const float base_hz =
          65.0f + host_cut * (100.0f + clean * 380.0f + vibe_ * 5200.0f);
      const float env_hz = base_hz + filt_env_ * (480.0f + vibe_ * 2200.0f);
      const float res_use = host_res * (0.35f + vibe_ * 0.45f);

      sig = phatty_ladder_lp(sig, env_hz, res_use);
      sig *= amp_;
    }

    phase1_ += w1;
    phase2_ += w2;
    phase_sub_ += w_sub;
    out[0] = sig;
  }
  }

private:
  float w0_;
  float w_tgt_;
  float host_cut_;
  float host_res_;

  uint32_t phase1_;
  uint32_t phase2_;
  uint32_t phase_sub_;
  uint32_t pwm_lfo_;
  float w_cur_;
  uint8_t env_stage_;
  uint8_t gate_;
  float amp_;
  float filt_env_;
  float vibe_;
  float osc_mix_;
  float detune_;
  float sub_lvl_;
  float drive_;
  float glide_;
  float attack_sec_;
  float decay_sec_;
  float release_sec_;
  float sustain_lvl_;
  float lp1_;
  float lp2_;
  float lp3_;
  float lp4_;

  float phatty_norm100(uint16_t value) {
    if (value > 100U) {
      value = 100U;
    }
    return (float)value * 0.01f;
  }

  float phatty_time_sec(uint16_t value, float min_s, float max_s) {
    const float n = phatty_norm100(value);
    return min_s + n * n * max_s;
  }

  uint32_t phatty_float_to_w(float w) {
    if (w < 0.0f) {
      w = 0.0f;
    }
    if (w > 4294967295.0f) {
      w = 4294967295.0f;
    }
    return (uint32_t)w;
  }

  float phatty_saw(uint32_t phase) {
    return (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
  }

  float phatty_square(uint32_t phase) {
    return (phase & 0x80000000U) ? 1.0f : -1.0f;
  }

  float phatty_pulse(uint32_t phase, float width) {
    if (width < 0.08f) {
      width = 0.08f;
    }
    if (width > 0.92f) {
      width = 0.92f;
    }
    const uint32_t th = (uint32_t)(width * 4294967295.0f);
    return (phase < th) ? 1.0f : -1.0f;
  }

  float phatty_lfo(uint32_t ph) {
    const float norm = (float)ph * (1.0f / 4294967296.0f);
    return sinf(k_two_pi * norm);
  }

  float phatty_one_pole_g(float hz) {
    if (hz < 35.0f) {
      hz = 35.0f;
    }
    if (hz > 18000.0f) {
      hz = 18000.0f;
    }
    return 1.0f - expf(-k_two_pi * hz / k_fs);
  }

  float phatty_multidrive(float in) {
    const float amount = drive_ * (0.35f + vibe_ * 0.75f);
    if (amount < 0.01f) {
      return in;
    }
    const float gain = 1.0f + amount * 7.0f;
    float x = in * gain;
    if (x > 0.0f) {
      x = tanhf(x * (1.0f + amount * 0.4f));
    } else {
      x = tanhf(x * (1.0f + amount * 0.25f)) * (1.0f - amount * 0.08f);
    }
    return x * (1.0f / (1.0f + amount * 0.35f));
  }

  float phatty_ladder_lp(float in, float cutoff_hz, float res_norm) {
    const float g = phatty_one_pole_g(cutoff_hz);
    const float k = 3.8f * res_norm;
    const float fb = lp4_ * (0.12f + res_norm * 0.82f);
    float x = in - fb * k;

    lp1_ += g * (x - lp1_);
    lp2_ += g * (lp1_ - lp2_);
    lp3_ += g * (lp2_ - lp3_);
    lp4_ += g * (lp3_ - lp4_);
    return lp4_;
  }

  void updateSustain() {
    const float clean = 1.0f - fminf(vibe_ * 1.1f, 1.0f);
    const float mean = fminf(fmaxf((vibe_ - 0.35f) * 1.5f, 0.0f), 1.0f);
    sustain_lvl_ = 0.55f * clean + 0.82f * mean + fminf(vibe_ * 0.15f, 0.12f);
    if (sustain_lvl_ > 0.94f) {
      sustain_lvl_ = 0.94f;
    }
  }
};
