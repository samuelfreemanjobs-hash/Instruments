#pragma once
/* PORT_COMPLETE — Detroit rap / electro 303-style bass osc (slide + accent). */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum {
    WAVE = 0U,
    SLIDE = 1U,
    ACCENT = 2U,
    ENVAMT = 3U,
    DECAY = 4U,
    DRIVE = 5U,
    GLIDE = 6U,
  };

  void setParameter(uint8_t index, int32_t value) override final {
    const float n = param_10bit_to_f32(value);
    switch (index) {
    case WAVE:
      wave_mix_ = n;
      break;
    case SLIDE:
      slide_time_ = 0.002f + n * n * 0.35f;
      break;
    case ACCENT:
      accent_amt_ = n;
      break;
    case ENVAMT:
      env_amt_ = n;
      break;
    case DECAY:
      decay_sec_ = 0.05f + n * n * 1.8f;
      break;
    case DRIVE:
      drive_ = n;
      break;
    case GLIDE:
      glide_ = n;
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t, int32_t) const override final { return nullptr; }
  void init(float *) override final { reset(); }

  void reset() override final {
    phase_ = 0.f;
    w0_ = w_cur_ = w_tgt_ = 0.f;
    env_ = accent_env_ = 0.f;
    gate_ = false;
    accent_pending_ = false;
    wave_mix_ = param_10bit_to_f32(184);
    slide_time_ = 0.08f;
    accent_amt_ = param_10bit_to_f32(614);
    env_amt_ = param_10bit_to_f32(532);
    decay_sec_ = 0.35f;
    drive_ = param_10bit_to_f32(409);
    glide_ = param_10bit_to_f32(368);
  }

  void setPitch(float w0) { w0_ = w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void noteOn(uint8_t, uint8_t velo) override final {
    gate_ = true;
    accent_pending_ = (velo > 90) || (accent_amt_ > 0.55f);
    w_tgt_ = w0_;
    if (slide_time_ < 0.005f || w_cur_ < 1e-9f) {
      w_cur_ = w0_;
    }
    if (env_ < 0.01f) {
      env_ = 0.01f;
    }
  }

  void noteOff(uint8_t) override final {
    gate_ = false;
  }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    static constexpr float k_fs = 48000.f;
    (void)in;
    const float slide_coef =
        (slide_time_ > 0.001f) ? (1.f / (slide_time_ * k_fs)) : 1.f;
    const float decay_coef = expf(-6.f / (decay_sec_ * k_fs));
    const float accent_attack = 0.015f + (1.f - accent_amt_) * 0.04f;

    for (const float *out_end = out + frames; out != out_end; ++out) {
      w_cur_ += (w_tgt_ - w_cur_) * slide_coef * (0.3f + glide_);

      if (gate_) {
        env_ += (1.f - env_) * 0.08f;
      } else {
        env_ *= decay_coef;
        if (env_ < 0.0001f) {
          env_ = 0.f;
        }
      }

      if (accent_pending_) {
        accent_env_ += accent_attack;
        if (accent_env_ >= 1.f) {
          accent_env_ = 1.f;
          accent_pending_ = false;
        }
      } else {
        accent_env_ *= 0.9992f;
      }

      float sig = 0.f;
      if (env_ > 0.0001f) {
        phase_ += w_cur_;
        if (phase_ >= 1.f) {
          phase_ -= floorf(phase_);
        }
        const float saw = 2.f * phase_ - 1.f;
        const float square = (phase_ < 0.5f) ? 1.f : -1.f;
        sig = saw * (1.f - wave_mix_) + square * wave_mix_;
        const float accent = 1.f + accent_env_ * accent_amt_ * env_amt_ * 0.85f;
        sig *= env_ * accent;
        const float g = 1.f + drive_ * 4.f;
        sig = tanhf(sig * g);
      }
      *out = sig;
    }
  }

private:
  float phase_;
  float w0_, w_cur_, w_tgt_;
  float env_, accent_env_;
  bool gate_;
  bool accent_pending_;
  float wave_mix_, slide_time_, accent_amt_, env_amt_, decay_sec_, drive_, glide_;
};
