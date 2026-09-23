#pragma once
/* PORT_COMPLETE — OB-Xa-ish dual saw/pulse stack. */
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
      mix_ = n;
      break;
    case 1:
      detune_ = n;
      break;
    case 2:
      pwm_ = n;
      break;
    case 3:
      brute_ = n;
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
    ph_a_ = ph_b_ = 0.f;
    w0_ = w_cur_ = 0.f;
    amp_ = 1.f;
    mix_ = param_10bit_to_f32(512);
    detune_ = param_10bit_to_f32(368);
    pwm_ = param_10bit_to_f32(450);
    brute_ = param_10bit_to_f32(409);
    glide_ = param_10bit_to_f32(286);
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
    const float wb = w_cur_ * (1.f + detune_ * 0.018f);

    for (const float *out_end = out + frames; out != out_end; ++out) {
      w_cur_ += (w0_ - w_cur_) * glide_coef;
      ph_a_ += w_cur_;
      ph_b_ += wb;
      if (ph_a_ >= 1.f) {
        ph_a_ -= floorf(ph_a_);
      }
      if (ph_b_ >= 1.f) {
        ph_b_ -= floorf(ph_b_);
      }
      const float saw_a = 2.f * ph_a_ - 1.f;
      const float saw_b = 2.f * ph_b_ - 1.f;
      const float width = 0.35f + pwm_ * 0.4f;
      const float pulse_b = (ph_b_ < width) ? 1.f : -1.f;
      float sig = saw_a * (1.f - mix_) + (saw_b * 0.6f + pulse_b * 0.4f) * mix_;
      sig = tanhf(sig * (1.f + brute_ * 3.5f));
      *out = sig * amp_;
    }
  }

private:
  float ph_a_, ph_b_;
  float w0_, w_cur_, amp_;
  float mix_, detune_, pwm_, brute_, glide_;
};
