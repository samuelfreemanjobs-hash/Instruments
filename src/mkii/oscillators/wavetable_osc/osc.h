#pragma once
/* mkII oscillator — PORT DSP from v1: oscillators/wavetable/wavetable.cc
 * Replace process() body with your mkI OSC_CYCLE logic (float out, osc_w0f pitch).
 */
#include "processor.h"
#include "unit_osc.h"

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum {
    MORPH = 0U,
    NUM_PARAMS
  };

  struct Params {
    float p0;
    float p1;
    float p2;
    float p3;
    float p4;
    float p5;
    float p6;
    float p7;
    float p8;
    void reset() {
      p0 = param_10bit_to_f32(0);
    }
    Params() { reset(); }
  };

  void setParameter(uint8_t index, int32_t value) override final {
    switch (index) {
    case MORPH:
      params_.p0 = param_10bit_to_f32(value);
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t index, int32_t value) const override final {
    (void)index;
    (void)value;
    return nullptr;
  }

  void init(float *) override final {
    params_.reset();
    phasor_ = 0.f;
    w0_ = 0.f;
  }

  void setPitch(float w0) { w0_ = w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    const Params p = params_;
    for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
      phasor_ = fmodf(phasor_ + w0_, 1.f);
      const float shape = p.p0;
      const float saw = 2.f * phasor_ - 1.f;
      out[0] = osc_sinf(phasor_) * (1.f - shape) + saw * shape;
    }
  }

private:
  Params params_;
  float w0_;
  float phasor_;
};
