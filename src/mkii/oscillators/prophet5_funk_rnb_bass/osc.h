#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * mkII port of src/oscillators/prophet-funk/prophet_bass.cpp
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
    SYNC = 3U,
    PWM = 4U,
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
    case SYNC:
      sync_amt_ = norm;
      break;
    case PWM:
      pwm_depth_ = norm;
      break;
    case GLIDE:
      glide_ = norm;
      break;
    case ATTACK:
      attack_sec_ = timeSec(norm, 0.001f, 0.32f);
      break;
    case DECAY:
      decay_sec_ = timeSec(norm, 0.025f, 1.35f);
      break;
    case RELEASE:
      release_sec_ = timeSec(norm, 0.035f, 1.6f);
      break;
    default:
      break;
    }
  }

  void init(float *) override final { reset(); }

  void reset() override final {
    phase_a_ = 0U;
    phase_b_ = 0U;
    lfo_pwm_ = 0U;
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

  const float glide_coef = 0.00013f + glide_ * glide_ * 0.0095f;
  const float attack_step =
      (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * k_fs)) : 1.0f;
  const float decay_step =
      (decay_sec_ > 0.0001f) ? (1.0f / (decay_sec_ * k_fs)) : 1.0f;
  const float release_step =
      (release_sec_ > 0.0001f) ? (1.0f / (release_sec_ * k_fs)) : 1.0f;

  const float host_cut = host_cut_;
  const float host_res = host_res_;
  const uint32_t pwm_lfo_inc = 167496331U;

  const float funk_w = 1.0f - fminf(vibe_ * 0.55f, 1.0f);
  const float rb_w = fminf(fmaxf((vibe_ - 0.4f) * 1.65f, 0.0f), 1.0f);

  for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
    w_cur_ += (w_tgt_ - w_cur_) * glide_coef;

    const float det = 1.0f + detune_ * (0.006f + funk_w * 0.008f);
    const uint32_t w_a = prophet_float_to_w(w_cur_);
    const uint32_t w_b = prophet_float_to_w(w_cur_ * det);

    lfo_pwm_ += pwm_lfo_inc;

    const uint32_t pa_prev = phase_a_;
    phase_a_ += w_a;
    if (phase_a_ < pa_prev && sync_amt_ > 0.01f) {
      const float reset = sync_amt_ * (0.35f + funk_w * 0.65f);
      phase_b_ = (uint32_t)((float)phase_a_ * reset);
    } else {
      phase_b_ += w_b;
    }

    if (env_stage_ == 1) {
      amp_ += attack_step;
      filt_env_ += attack_step * (1.05f + funk_w * 0.35f);
      if (amp_ >= 1.0f) {
        amp_ = 1.0f;
        filt_env_ = fminf(filt_env_, 1.0f);
        env_stage_ = 2;
      }
    } else if (env_stage_ == 2) {
      amp_ += (sustain_lvl_ - amp_) * decay_step;
      filt_env_ += (sustain_lvl_ * 0.6f - filt_env_) * decay_step;
      if (fabsf(amp_ - sustain_lvl_) < 0.002f) {
        amp_ = sustain_lvl_;
        env_stage_ = 3;
      }
    } else if (env_stage_ == 3) {
      if (gate_ == 0U) {
        env_stage_ = 4;
      }
      amp_ = sustain_lvl_;
      filt_env_ = sustain_lvl_ * 0.6f;
    } else if (env_stage_ == 4) {
      amp_ -= release_step;
      filt_env_ -= release_step * 0.8f;
      if (amp_ < k_amp_floor) {
        amp_ = 0.0f;
        filt_env_ = 0.0f;
        env_stage_ = 0;
      }
    }

    float sig = 0.0f;
    if (env_stage_ != 0) {
      const float width =
          0.45f + pwm_depth_ * 0.22f + prophet_lfo(lfo_pwm_) * pwm_depth_ * 0.12f;
      const float saw_a = prophet_saw(phase_a_);
      const float saw_b = prophet_saw(phase_b_);
      const float pulse_a = prophet_pulse(phase_a_, width);
      const float pulse_b = prophet_pulse(phase_b_, 1.0f - width * 0.35f);

      const float saw_mix = saw_a * 0.55f + saw_b * (0.35f + detune_ * 0.2f);
      const float pulse_mix = pulse_a * 0.5f + pulse_b * 0.35f;
      sig = saw_mix * osc_mix_ + pulse_mix * (1.0f - osc_mix_ * 0.85f);

      const float osc_b_tone = saw_b * 0.4f + pulse_b * 0.3f;
      const float base_hz =
          75.0f + host_cut * (140.0f + funk_w * 2800.0f + rb_w * 4200.0f);
      const float poly_hz = osc_b_tone * poly_mod_ * (420.0f + rb_w * 380.0f);
      const float env_hz = base_hz + filt_env_ * (520.0f + funk_w * 1400.0f) + poly_hz;
      const float res_use = host_res * (0.28f + funk_w * 0.42f + rb_w * 0.22f);

      sig = prophet_cem_lp(sig, env_hz, res_use);
      sig *= amp_ * (0.92f + rb_w * 0.05f);
    }

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
  uint32_t lfo_pwm_;
  float w_cur_;
  uint8_t env_stage_;
  uint8_t gate_;
  float amp_;
  float filt_env_;
  float vibe_;
  float osc_mix_;
  float detune_;
  float sync_amt_;
  float pwm_depth_;
  float glide_;
  float attack_sec_;
  float decay_sec_;
  float release_sec_;
  float sustain_lvl_;
  float poly_mod_;
  float lp1_;
  float lp2_;
  float lp3_;
  float lp4_;

  float prophet_norm100(uint16_t value) {
    if (value > 100U) {
      value = 100U;
    }
    return (float)value * 0.01f;
  }

  float prophet_time_sec(uint16_t value, float min_s, float max_s) {
    const float n = prophet_norm100(value);
    return min_s + n * n * max_s;
  }

  uint32_t prophet_float_to_w(float w) {
    if (w < 0.0f) {
      w = 0.0f;
    }
    if (w > 4294967295.0f) {
      w = 4294967295.0f;
    }
    return (uint32_t)w;
  }

  float prophet_saw(uint32_t phase) {
    return (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
  }

  float prophet_pulse(uint32_t phase, float width) {
    if (width < 0.06f) {
      width = 0.06f;
    }
    if (width > 0.94f) {
      width = 0.94f;
    }
    const uint32_t th = (uint32_t)(width * 4294967295.0f);
    return (phase < th) ? 1.0f : -1.0f;
  }

  float prophet_lfo(uint32_t ph) {
    const float norm = (float)ph * (1.0f / 4294967296.0f);
    return sinf(k_two_pi * norm);
  }

  float prophet_one_pole_g(float hz) {
    if (hz < 30.0f) {
      hz = 30.0f;
    }
    if (hz > 16000.0f) {
      hz = 16000.0f;
    }
    return 1.0f - expf(-k_two_pi * hz / k_fs);
  }

  float prophet_cem_lp(float in, float cutoff_hz, float res_norm) {
    const float g = prophet_one_pole_g(cutoff_hz);
    const float res = res_norm * (0.55f + vibe_ * 0.35f);
    const float fb = (lp3_ + lp4_ * 0.5f) * (0.08f + res * 0.95f);
    float x = in - fb;

    lp1_ += g * (x - lp1_);
    lp2_ += g * (lp1_ - lp2_);
    lp3_ += g * (lp2_ - lp3_);
    lp4_ += g * (lp3_ - lp4_);
    return lp4_ * (1.0f + res * 0.08f);
  }

  void updateSustain() {
    const float funk70 = 1.0f - fminf(fmaxf((vibe_ - 0.38f) * 1.8f, 0.0f), 1.0f);
    const float rb80 = fminf(fmaxf((vibe_ - 0.32f) * 1.45f, 0.0f), 1.0f);
    sustain_lvl_ = 0.68f * funk70 + 0.76f * rb80 + 0.08f;
    if (sustain_lvl_ > 0.93f) {
      sustain_lvl_ = 0.93f;
    }
    poly_mod_ = 0.15f + rb80 * 0.35f + (1.0f - funk70) * 0.12f;
  }
};
