#pragma once
/* mkII oscillator — PORT DSP from v1: oscillators/khalifa-haze/khalifa_bass.cc
 * Replace process() body with your mkI OSC_CYCLE logic (float out, osc_w0f pitch).
 */
#include "processor.h"
#include "unit_osc.h"

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum {
    VIBE = 0U,
    SUB = 1U,
    HAZE = 2U,
    FLOAT = 3U,
    WARMTH = 4U,
    GLIDE = 5U,
    ATTACK = 6U,
    DECAY = 7U,
    RELEASE = 8U,
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
      p0 = param_10bit_to_f32(512);
      p1 = param_10bit_to_f32(737);
      p2 = param_10bit_to_f32(563);
      p3 = param_10bit_to_f32(593);
      p4 = param_10bit_to_f32(491);
      p5 = param_10bit_to_f32(563);
      p6 = param_10bit_to_f32(327);
      p7 = param_10bit_to_f32(532);
      p8 = param_10bit_to_f32(634);
    }
    Params() { reset(); }
  };

  void setParameter(uint8_t index, int32_t value) override final {
    switch (index) {
    case VIBE:
      params_.p0 = param_10bit_to_f32(value);
      break;
    case SUB:
      params_.p1 = param_10bit_to_f32(value);
      break;
    case HAZE:
      params_.p2 = param_10bit_to_f32(value);
      break;
    case FLOAT:
      params_.p3 = param_10bit_to_f32(value);
      break;
    case WARMTH:
      params_.p4 = param_10bit_to_f32(value);
      break;
    case GLIDE:
      params_.p5 = param_10bit_to_f32(value);
      break;
    case ATTACK:
      params_.p6 = param_10bit_to_f32(value);
      break;
    case DECAY:
      params_.p7 = param_10bit_to_f32(value);
      break;
    case RELEASE:
      params_.p8 = param_10bit_to_f32(value);
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
