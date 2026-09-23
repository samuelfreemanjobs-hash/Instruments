#pragma once
/* PORT_COMPLETE — mkII port of src/oscillators/sh101-babyface/sh101_bass.cc (simplified LP). */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  void setParameter(uint8_t index, int32_t value) override final {
    const float n = param_10bit_to_f32(value);
    switch (index) {
    case 0:
      vibe_ = n;
      sustain_lvl_ = 0.64f + n * 0.28f;
      break;
    case 1:
      pwm_depth_ = n;
      break;
    case 2:
      sub_lvl_ = n;
      break;
    case 3:
      hollow_ = n;
      break;
    case 4:
      saw_mix_ = n;
      break;
    case 5:
      glide_ = n;
      break;
    case 6:
      attack_sec_ = 0.002f + n * n * 0.28f;
      break;
    case 7:
      decay_sec_ = 0.03f + n * n * 1.2f;
      break;
    case 8:
      release_sec_ = 0.04f + n * n * 1.5f;
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t, int32_t) const override final { return nullptr; }
  void init(float *) override final { reset(); }

  void reset() override final {
    phase_ = phase_sub_ = phase_mod_ = pwm_lfo_ = 0.f;
    w0_ = w_cur_ = 0.f;
    amp_ = filt_env_ = fm_env_ = 0.f;
    env_stage_ = 0;
    gate_ = false;
    lp1_ = lp2_ = lp3_ = lp4_ = 0.f;
    vibe_ = param_10bit_to_f32(307);
    pwm_depth_ = param_10bit_to_f32(471);
    sub_lvl_ = param_10bit_to_f32(532);
    hollow_ = param_10bit_to_f32(286);
    saw_mix_ = param_10bit_to_f32(184);
    glide_ = param_10bit_to_f32(245);
    attack_sec_ = 0.01f;
    decay_sec_ = 0.4f;
    release_sec_ = 0.45f;
    sustain_lvl_ = 0.72f;
    tone_hz_ = 800.f;
  }

  void setPitch(float w0) { w0_ = w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void noteOn(uint8_t, uint8_t) override final {
    gate_ = true;
    if (glide_ < 0.02f) {
      w_cur_ = w0_;
    }
    fm_env_ = 1.f;
    if (env_stage_ == 0 || env_stage_ == 4) {
      env_stage_ = 1;
    }
  }

  void noteOff(uint8_t) override final {
    gate_ = false;
    if (env_stage_ != 0) {
      env_stage_ = 4;
    }
  }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    static constexpr float k_fs = 48000.f;
    static constexpr float k_two_pi = 6.28318530718f;
    (void)in;
    const float glide_coef = 0.00012f + glide_ * glide_ * 0.009f;
    const float attack_step = 1.f / (attack_sec_ * k_fs);
    const float decay_step = 1.f / (decay_sec_ * k_fs);
    const float release_step = 1.f / (release_sec_ * k_fs);
    const float pwm_lfo_inc = 118496331.f / 4294967296.f / k_fs;

    for (const float *out_end = out + frames; out != out_end; ++out) {
      w_cur_ += (w0_ - w_cur_) * glide_coef;
      pwm_lfo_ += pwm_lfo_inc;
      if (pwm_lfo_ >= 1.f) {
        pwm_lfo_ -= 1.f;
      }
      fm_env_ -= fm_env_ * (0.00012f + hollow_ * 0.0018f);
      if (fm_env_ < 0.08f) {
        fm_env_ = 0.08f;
      }

      if (env_stage_ == 1) {
        amp_ += attack_step;
        filt_env_ += attack_step * 0.9f;
        if (amp_ >= 1.f) {
          amp_ = 1.f;
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
        amp_ = sustain_lvl_;
        filt_env_ = sustain_lvl_ * 0.5f;
        if (!gate_) {
          env_stage_ = 4;
        }
      } else if (env_stage_ == 4) {
        amp_ -= release_step;
        if (amp_ < 0.00008f) {
          amp_ = 0.f;
          env_stage_ = 0;
        }
      }

      float sig = 0.f;
      if (env_stage_ != 0) {
        phase_ += w_cur_;
        phase_sub_ += w_cur_ * 0.5f;
        phase_mod_ += w_cur_ * (1.f + hollow_ * 0.5f);
        if (phase_ >= 1.f) {
          phase_ -= floorf(phase_);
        }
        if (phase_sub_ >= 1.f) {
          phase_sub_ -= floorf(phase_sub_);
        }
        if (phase_mod_ >= 1.f) {
          phase_mod_ -= floorf(phase_mod_);
        }

        const float warm = 1.f - fminf(vibe_ * 0.9f, 1.f);
        const float hollow_mix = fminf(fmaxf((vibe_ - 0.38f) * 1.6f, 0.f), 1.f);
        const float width =
            0.52f - warm * 0.08f + sinf(k_two_pi * pwm_lfo_) * pwm_depth_ * 0.14f;
        const float pulse = (phase_ < width) ? 1.f : -1.f;
        const float sub = (phase_sub_ < 0.5f) ? 1.f : -1.f;
        const float saw = (2.f * phase_ - 1.f) * saw_mix_ * (0.25f + warm * 0.35f);
        const float mod = sinf(k_two_pi * phase_mod_) * hollow_ * fm_env_ * (1.2f + hollow_mix * 2.4f);
        float car = k_two_pi * phase_ + mod * (0.35f + hollow_mix * 0.45f);
        const float fm_voice = sinf(car) * hollow_mix * 0.55f;

        sig = pulse * (0.62f + warm * 0.28f);
        sig += sub * sub_lvl_ * (0.48f + warm * 0.2f);
        sig += saw;
        sig += fm_voice * (0.4f + hollow_ * 0.5f);
        const float cut = tone_hz_ + filt_env_ * (400.f + warm * 1100.f);
        sig = rolandLp(sig, cut, warm * 0.5f);
        sig *= amp_ * 0.9f;
      }
      *out = sig;
    }
  }

private:
  float rolandLp(float in, float hz, float res_norm) {
    const float g = 1.f - expf(-6.28318530718f * hz / 48000.f);
    const float fb = lp4_ * (0.1f + res_norm * 0.65f);
    float x = in - fb;
    lp1_ += g * (x - lp1_);
    lp2_ += g * (lp1_ - lp2_);
    lp3_ += g * (lp2_ - lp3_);
    lp4_ += g * (lp3_ - lp4_);
    return lp4_;
  }

  static constexpr float k_two_pi = 6.28318530718f;

  float phase_, phase_sub_, phase_mod_, pwm_lfo_;
  float w0_, w_cur_;
  float amp_, filt_env_, fm_env_;
  uint8_t env_stage_;
  bool gate_;
  float sustain_lvl_;
  float lp1_, lp2_, lp3_, lp4_;
  float vibe_, pwm_depth_, sub_lvl_, hollow_, saw_mix_, glide_;
  float attack_sec_, decay_sec_, release_sec_;
  float tone_hz_;
};
