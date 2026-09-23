#pragma once
/* PORT_COMPLETE — MS-20-style hard sync saws + scream drive. */
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
      sync_depth_ = n;
      break;
    case 1:
      ratio_ = 0.5f + n * 3.5f;
      break;
    case 2:
      scream_ = n;
      break;
    case 3:
      mix_ = n;
      break;
    case 4:
      glide_ = n;
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t, int32_t) const override final { return nullptr; }
  void init(float *) override final { reset(); }

  void reset() override final {
    ph_m_ = ph_s_ = 0.f;
    w0_ = w_cur_ = 0.f;
    sync_depth_ = param_10bit_to_f32(614);
    ratio_ = 2.2f;
    scream_ = param_10bit_to_f32(532);
    mix_ = param_10bit_to_f32(512);
    glide_ = param_10bit_to_f32(245);
  }

  void setPitch(float w0) { w0_ = w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void noteOn(uint8_t, uint8_t) override final {
    if (glide_ < 0.02f) {
      w_cur_ = w0_;
    }
  }

  void noteOff(uint8_t) override final {}

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    (void)in;
    const float glide_coef = 0.00012f + glide_ * glide_ * 0.005f;

    for (const float *out_end = out + frames; out != out_end; ++out) {
      w_cur_ += (w0_ - w_cur_) * glide_coef;
      const float wm = w_cur_;
      const float ws = wm * ratio_;

      ph_m_ += wm;
      if (ph_m_ >= 1.f) {
        ph_m_ -= floorf(ph_m_);
        ph_s_ = 0.f;
      }
      ph_s_ += ws;
      if (ph_s_ >= 1.f) {
        ph_s_ -= floorf(ph_s_);
      }

      const float saw_m = 2.f * ph_m_ - 1.f;
      const float saw_s = 2.f * ph_s_ - 1.f;
      const float blend = mix_ * (0.35f + sync_depth_ * 0.65f);
      float sig = saw_m * (1.f - blend) + saw_s * blend;
      sig = tanhf(sig * (1.f + scream_ * 4.5f));
      *out = sig;
    }
  }

private:
  float ph_m_, ph_s_;
  float w0_, w_cur_;
  float sync_depth_, ratio_, scream_, mix_, glide_;
};
