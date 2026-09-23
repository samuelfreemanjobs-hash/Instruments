#pragma once
/* PORT_COMPLETE — Plug/Nether short pluck + sub. */
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
      pluck_ = n;
      break;
    case 1:
      decay_ = 0.025f + n * n * 0.35f;
      break;
    case 2:
      tone_ = n;
      break;
    case 3:
      sub_ = n;
      break;
    case 4:
      detune_ = n;
      break;
    case 5:
      glide_ = n;
      break;
    case 6:
      drive_ = n;
      break;
    case 7:
      level_ = 0.3f + n * 0.7f;
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t, int32_t) const override final { return nullptr; }
  void init(float *) override final { reset(); }

  void reset() override final {
    ph_ = sub_ph_ = 0.f;
    w0_ = w_cur_ = 0.f;
    env_ = 0.f;
    lp_ = 0.f;
    pluck_ = param_10bit_to_f32(655);
    decay_ = 0.12f;
    tone_ = param_10bit_to_f32(450);
    sub_ = param_10bit_to_f32(614);
    detune_ = param_10bit_to_f32(286);
    glide_ = param_10bit_to_f32(184);
    drive_ = param_10bit_to_f32(409);
    level_ = 0.85f;
  }

  void setPitch(float w0) { w0_ = w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void noteOn(uint8_t, uint8_t velo) override final {
    env_ = static_cast<float>(velo) / 127.f * (0.7f + pluck_ * 0.3f);
    if (glide_ < 0.02f) {
      w_cur_ = w0_;
    }
  }

  void noteOff(uint8_t) override final {}

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    static constexpr float k_fs = 48000.f;
    (void)in;
    const float glide_c = 0.00015f + glide_ * glide_ * 0.004f;
    const float dcoef = expf(-6.f / (decay_ * k_fs));
    const float lp_g = 1.f - expf(-6.28318530718f * (120.f + tone_ * 2800.f) / k_fs);

    for (const float *out_end = out + frames; out != out_end; ++out) {
      w_cur_ += (w0_ * (1.f + detune_ * 0.008f) - w_cur_) * glide_c;
      ph_ += w_cur_;
      sub_ph_ += w_cur_ * 0.5f;
      if (ph_ >= 1.f) {
        ph_ -= floorf(ph_);
      }
      if (sub_ph_ >= 1.f) {
        sub_ph_ -= floorf(sub_ph_);
      }

      float s = sinf(6.28318530718f * ph_);
      s += (2.f * ph_ - 1.f) * tone_ * 0.25f;
      s += sinf(6.28318530718f * sub_ph_) * sub_ * 0.55f;

      lp_ += lp_g * (s - lp_);
      env_ *= dcoef;
      s = tanhf(lp_ * env_ * (1.f + drive_ * 2.f) * level_);
      *out = s;
    }
  }

private:
  float ph_, sub_ph_, w0_, w_cur_, env_, lp_;
  float pluck_, decay_, tone_, sub_, detune_, glide_, drive_, level_;
};
