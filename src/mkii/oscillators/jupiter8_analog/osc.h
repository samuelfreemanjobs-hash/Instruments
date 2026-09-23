#pragma once
/* PORT_COMPLETE — mkII port of src/oscillators/prophet-funk/prophet_bass.cc (dual osc + LP). */
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
      sustain_lvl_ = 0.6f + n * 0.32f;
      break;
    case 1:
      osc_mix_ = n;
      break;
    case 2:
      detune_ = n;
      break;
    case 3:
      sync_amt_ = n;
      break;
    case 4:
      pwm_depth_ = n;
      break;
    case 5:
      glide_ = n;
      break;
    case 6:
      attack_sec_ = 0.002f + n * n * 0.3f;
      break;
    case 7:
      decay_sec_ = 0.03f + n * n * 1.3f;
      break;
    case 8:
      release_sec_ = 0.04f + n * n * 1.6f;
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t, int32_t) const override final { return nullptr; }
  void init(float *) override final { reset(); }

  void reset() override final {
    phase_a_ = phase_b_ = pwm_lfo_ = 0.f;
    w0_ = w_cur_ = 0.f;
    amp_ = filt_env_ = 0.f;
    env_stage_ = 0;
    gate_ = false;
    lp1_ = lp2_ = lp3_ = lp4_ = 0.f;
    vibe_ = param_10bit_to_f32(409);
    osc_mix_ = param_10bit_to_f32(614);
    detune_ = param_10bit_to_f32(245);
    sync_amt_ = param_10bit_to_f32(184);
    pwm_depth_ = param_10bit_to_f32(327);
    glide_ = param_10bit_to_f32(286);
    attack_sec_ = 0.012f;
    decay_sec_ = 0.45f;
    release_sec_ = 0.5f;
    sustain_lvl_ = 0.75f;
  }

  void setPitch(float w0) { w0_ = w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void noteOn(uint8_t, uint8_t) override final {
    gate_ = true;
    if (glide_ < 0.02f) {
      w_cur_ = w0_;
    }
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
    const float glide_coef = 0.0001f + glide_ * glide_ * 0.006f;
    const float attack_step = 1.f / (attack_sec_ * k_fs);
    const float decay_step = 1.f / (decay_sec_ * k_fs);
    const float release_step = 1.f / (release_sec_ * k_fs);
    const float pwm_inc = 0.6f / k_fs;

    for (const float *out_end = out + frames; out != out_end; ++out) {
      w_cur_ += (w0_ - w_cur_) * glide_coef;
      pwm_lfo_ += pwm_inc;
      if (pwm_lfo_ >= 1.f) {
        pwm_lfo_ -= 1.f;
      }

      if (env_stage_ == 1) {
        amp_ += attack_step;
        filt_env_ += attack_step;
        if (amp_ >= 1.f) {
          amp_ = 1.f;
          env_stage_ = 2;
        }
      } else if (env_stage_ == 2) {
        amp_ += (sustain_lvl_ - amp_) * decay_step;
        filt_env_ += (sustain_lvl_ - filt_env_) * decay_step;
        if (fabsf(amp_ - sustain_lvl_) < 0.002f) {
          amp_ = sustain_lvl_;
          env_stage_ = 3;
        }
      } else if (env_stage_ == 3) {
        amp_ = sustain_lvl_;
        filt_env_ = sustain_lvl_;
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
        const float wb = w_cur_ * (1.f + detune_ * 0.015f);
        phase_a_ += w_cur_;
        phase_b_ += wb;
        if (phase_a_ >= 1.f) {
          phase_a_ -= floorf(phase_a_);
        }
        if (phase_b_ >= 1.f) {
          phase_b_ -= floorf(phase_b_);
        }
        if (sync_amt_ > 0.01f) {
          phase_b_ = phase_a_ * (1.f + sync_amt_ * 2.f);
          phase_b_ -= floorf(phase_b_);
        }
        const float width = 0.5f + sinf(k_two_pi * pwm_lfo_) * pwm_depth_ * 0.12f;
        const float saw_a = 2.f * phase_a_ - 1.f;
        const float saw_b = 2.f * phase_b_ - 1.f;
        const float pulse_b = (phase_b_ < width) ? 1.f : -1.f;
        sig = saw_a * (1.f - osc_mix_) + (saw_b * 0.7f + pulse_b * 0.3f) * osc_mix_;
        const float cut = 120.f + vibe_ * 4200.f + filt_env_ * 800.f;
        sig = filterOut(sig, cut);
        sig *= amp_ * 0.88f;
      }
      *out = sig;
    }
  }

private:
  float filterOut(float in, float hz) {
    const float g = 1.f - expf(-6.28318530718f * hz / 48000.f);
    const float fb = lp4_ * (0.08f + vibe_ * 0.5f);
    float x = in - fb;
    lp1_ += g * (x - lp1_);
    lp2_ += g * (lp1_ - lp2_);
    lp3_ += g * (lp2_ - lp3_);
    lp4_ += g * (lp3_ - lp4_);
    return lp4_;
  }

  float phase_a_, phase_b_, pwm_lfo_;
  float w0_, w_cur_;
  float amp_, filt_env_;
  uint8_t env_stage_;
  bool gate_;
  float sustain_lvl_;
  float lp1_, lp2_, lp3_, lp4_;
  float vibe_, osc_mix_, detune_, sync_amt_, pwm_depth_, glide_;
  float attack_sec_, decay_sec_, release_sec_;
};
