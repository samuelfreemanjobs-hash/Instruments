#pragma once
/* PORT_COMPLETE — Minimoog-style 3-osc stack + saturation. */
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
      osc_mix_ = n;
      break;
    case 1:
      drive_ = n;
      break;
    case 2:
      glide_ = n;
      break;
    case 3:
      attack_ = 0.001f + n * n * 0.2f;
      break;
    case 4:
      release_ = 0.02f + n * n * 1.2f;
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t, int32_t) const override final { return nullptr; }
  void init(float *) override final { reset(); }

  void reset() override final {
    ph16_ = ph8_ = ph4_ = 0.f;
    w0_ = w_cur_ = 0.f;
    amp_ = 0.f;
    env_stage_ = 0;
    gate_ = false;
    osc_mix_ = param_10bit_to_f32(614);
    drive_ = param_10bit_to_f32(532);
    glide_ = param_10bit_to_f32(245);
    attack_ = 0.005f;
    release_ = 0.35f;
  }

  void setPitch(float w0) { w0_ = w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void noteOn(uint8_t, uint8_t) override final {
    gate_ = true;
    env_stage_ = 1;
  }

  void noteOff(uint8_t) override final {
    gate_ = false;
    env_stage_ = 2;
  }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    static constexpr float k_fs = 48000.f;
    (void)in;
    const float glide_coef = 0.00015f + glide_ * glide_ * 0.006f;

    for (const float *out_end = out + frames; out != out_end; ++out) {
      w_cur_ += (w0_ - w_cur_) * glide_coef;
      if (env_stage_ == 1) {
        amp_ += 1.f / (attack_ * k_fs);
        if (amp_ >= 1.f) {
          amp_ = 1.f;
          env_stage_ = gate_ ? 3 : 2;
        }
      } else if (env_stage_ == 2) {
        amp_ -= 1.f / (release_ * k_fs);
        if (amp_ < 0.f) {
          amp_ = 0.f;
          env_stage_ = 0;
        }
      } else if (env_stage_ == 3 && !gate_) {
        env_stage_ = 2;
      }

      ph16_ += w_cur_ * 0.5f;
      ph8_ += w_cur_;
      ph4_ += w_cur_ * 2.f;
      if (ph16_ >= 1.f) {
        ph16_ -= floorf(ph16_);
      }
      if (ph8_ >= 1.f) {
        ph8_ -= floorf(ph8_);
      }
      if (ph4_ >= 1.f) {
        ph4_ -= floorf(ph4_);
      }

      const float s16 = 2.f * ph16_ - 1.f;
      const float s8 = 2.f * ph8_ - 1.f;
      const float s4 = 2.f * ph4_ - 1.f;
      float sig = (s16 * 0.45f + s8 * 0.35f + s4 * 0.2f) * (0.7f + osc_mix_ * 0.3f);
      sig = tanhf(sig * (1.f + drive_ * 5.f)) * amp_;
      *out = sig;
    }
  }

private:
  float ph16_, ph8_, ph4_;
  float w0_, w_cur_, amp_;
  uint8_t env_stage_;
  bool gate_;
  float osc_mix_, drive_, glide_, attack_, release_;
};
