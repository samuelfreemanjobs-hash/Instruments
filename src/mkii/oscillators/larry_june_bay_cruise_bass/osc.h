#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * mkII port of src/oscillators/larry-june/larry_bass.cpp
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
    PULSE = 2U,
    RUBBER = 3U,
    GLIDE = 4U,
    SPACE = 5U,
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
    case PULSE:
      pulse_width_ = norm;
      break;
    case RUBBER:
      rubber_ = norm;
      break;
    case GLIDE:
      glide_ = norm;
      break;
    case SPACE:
      space_ = norm;
      break;
    case ATTACK:
      attack_sec_ = timeSec(norm, 0.006f, 0.38f);
      break;
    case DECAY:
      decay_sec_ = timeSec(norm, 0.04f, 1.35f);
      break;
    case RELEASE:
      release_sec_ = timeSec(norm, 0.05f, 1.7f);
      break;
    default:
      break;
    }
  }

  void init(float *) override final { reset(); }

  void reset() override final {
    phase_a_ = 0U;
    phase_b_ = 0U;
    w_cur_ = 0.0f;
    w_tgt_ = 0.0f;
    env_stage_ = 0;
    gate_ = 0U;
    amp_ = 0.0f;
    filt_track_ = 0.0f;
    lp1_ = 0.0f;
    lp2_ = 0.0f;
    hp_z_ = 0.0f;
    ch_wp_ = 0U;
    for (uint32_t i = 0; i < 320U; i++) {
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

  const float glide_coef = 0.00007f + glide_ * glide_ * 0.008f;
  const float attack_step =
      (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * k_fs)) : 1.0f;
  const float decay_step =
      (decay_sec_ > 0.0001f) ? (1.0f / (decay_sec_ * k_fs)) : 1.0f;
  const float release_step =
      (release_sec_ > 0.0001f) ? (1.0f / (release_sec_ * k_fs)) : 1.0f;

  const float host_cut = host_cut_;
  const float host_res = host_res_;
  const uint32_t pwm_inc = 87349631U + (uint32_t)(pwm_depth_ * 1200000.0f);
  const uint32_t rubber_inc = 45249631U;

  for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
    w_cur_ += (w_tgt_ - w_cur_) * glide_coef;

    const float rubber_det = 1.0f + larry_lfo(rubber_lfo_) * rubber_ * 0.005f;
    const uint32_t w_a = larry_float_to_w(w_cur_);
    const uint32_t w_b = larry_float_to_w(w_cur_ * (1.0028f + rubber_ * 0.004f) * rubber_det);

    pwm_lfo_ += pwm_inc;
    rubber_lfo_ += rubber_inc;

    if (env_stage_ == 1) {
      amp_ += attack_step;
      filt_track_ += attack_step * 0.65f;
      if (amp_ >= 1.0f) {
        amp_ = 1.0f;
        filt_track_ = fminf(filt_track_, 1.0f);
        env_stage_ = 2;
      }
    } else if (env_stage_ == 2) {
      amp_ += (sustain_lvl_ - amp_) * decay_step;
      filt_track_ += (sustain_lvl_ * 0.4f - filt_track_) * decay_step;
      if (fabsf(amp_ - sustain_lvl_) < 0.002f) {
        amp_ = sustain_lvl_;
        env_stage_ = 3;
      }
    } else if (env_stage_ == 3) {
      if (gate_ == 0U) {
        env_stage_ = 4;
      }
      amp_ = sustain_lvl_;
      filt_track_ = sustain_lvl_ * 0.4f;
    } else if (env_stage_ == 4) {
      amp_ -= release_step;
      filt_track_ -= release_step * 0.55f;
      if (amp_ < k_amp_floor) {
        amp_ = 0.0f;
        filt_track_ = 0.0f;
        env_stage_ = 0;
      }
    }

    float sig = 0.0f;
    if (env_stage_ != 0) {
      const float base_w = 0.38f + pulse_width_ * 0.24f;
      const float width =
          base_w + larry_lfo(pwm_lfo_) * pwm_depth_ * (0.14f + vibe_ * 0.1f);
      const float pulse_a = larry_pulse(phase_a_, width);
      const float pulse_b = larry_pulse(phase_b_, 1.0f - width * 0.35f);

      const float phatt = 1.0f - fminf(space_ * 0.35f, 0.35f);
      sig = pulse_a * (0.58f + phatt * 0.15f) + pulse_b * (0.32f + rubber_ * 0.18f);

      const float cruise = 1.0f - fminf(vibe_ * 0.55f, 0.55f);
      float cut_hz = 90.0f + host_cut * (200.0f + cruise * 1800.0f + vibe_ * 2400.0f);
      cut_hz += filt_track_ * (180.0f + rubber_ * 520.0f);
      cut_hz += larry_lfo(pwm_lfo_) * rubber_ * 120.0f;

      const float g = larry_one_pole_g(cut_hz);
      const float res = host_res * (0.08f + rubber_ * 0.18f);
      const float fb = lp2_ * res;
      lp1_ += g * ((sig - fb) - lp1_);
      lp2_ += g * (lp1_ - lp2_);
      sig = lp2_;

      sig = larry_space_hpf(sig);
      sig = larry_chorus(sig);
      sig *= amp_ * (0.78f + cruise * 0.12f);
    }

    phase_a_ += w_a;
    phase_b_ += w_b;
    out[0] = sig;
  }
  }

private:
  float w0_;
  float w_tgt_;
  float host_cut_;
  float host_res_;

  uint32_t phase_a_;
  uint32_t phase_b_;
  uint32_t pwm_lfo_;
  uint32_t rubber_lfo_;
  float w_cur_;
  uint8_t env_stage_;
  uint8_t gate_;
  float amp_;
  float filt_track_;
  float vibe_;
  float pwm_depth_;
  float pulse_width_;
  float rubber_;
  float glide_;
  float space_;
  float attack_sec_;
  float decay_sec_;
  float release_sec_;
  float sustain_lvl_;
  float lp1_;
  float lp2_;
  float hp_z_;
  uint32_t ch_wp_;
  float ch_buf_[320];

  float larry_norm100(uint16_t value) {
    if (value > 100U) {
      value = 100U;
    }
    return (float)value * 0.01f;
  }

  float larry_time_sec(uint16_t value, float min_s, float max_s) {
    const float n = larry_norm100(value);
    return min_s + n * n * max_s;
  }

  uint32_t larry_float_to_w(float w) {
    if (w < 0.0f) {
      w = 0.0f;
    }
    if (w > 4294967295.0f) {
      w = 4294967295.0f;
    }
    return (uint32_t)w;
  }

  float larry_pulse(uint32_t phase, float width) {
    if (width < 0.12f) {
      width = 0.12f;
    }
    if (width > 0.88f) {
      width = 0.88f;
    }
    const uint32_t th = (uint32_t)(width * 4294967295.0f);
    return (phase < th) ? 1.0f : -1.0f;
  }

  float larry_lfo(uint32_t ph) {
    const float norm = (float)ph * (1.0f / 4294967296.0f);
    return sinf(k_two_pi * norm);
  }

  float larry_one_pole_g(float hz) {
    if (hz < 30.0f) {
      hz = 30.0f;
    }
    if (hz > 14000.0f) {
      hz = 14000.0f;
    }
    return 1.0f - expf(-k_two_pi * hz / k_fs);
  }

  float larry_space_hpf(float in) {
    const float amt = space_ * (0.45f + vibe_ * 0.25f);
    if (amt < 0.02f) {
      return in;
    }
    const float hz = 35.0f + amt * amt * 220.0f;
    const float g = larry_one_pole_g(hz);
    hp_z_ += g * (in - hp_z_);
    return in - hp_z_ * amt * 0.85f;
  }

  float larry_chorus(float in) {
    const float w = space_ * 0.22f;
    if (w < 0.01f) {
      return in;
    }
    const int32_t d = 22 + (int32_t)(larry_lfo(pwm_lfo_) * 8.0f);
    const uint32_t read = (ch_wp_ + 320U - (uint32_t)d) % 320U;
    const float wet = ch_buf_[read];
    ch_buf_[ch_wp_] = in;
    ch_wp_ = (ch_wp_ + 1U) % 320U;
    return in * (1.0f - w) + wet * w;
  }

  void updateSustain() {
    const float cruise = 1.0f - fminf(vibe_ * 0.4f, 0.4f);
    sustain_lvl_ = 0.58f * cruise + 0.72f * (1.0f - cruise) + vibe_ * 0.12f;
    if (sustain_lvl_ > 0.9f) {
      sustain_lvl_ = 0.9f;
    }
  }
};
