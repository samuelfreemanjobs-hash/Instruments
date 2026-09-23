#pragma once
/* PORT_COMPLETE — Drill 808: slide portamento + long tail release. */
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
      slide_ = 0.008f + n * n * 0.12f;
      break;
    case 1:
      tail_ = 0.08f + n * n * 1.8f;
      break;
    case 2:
      tune_ = 0.5f + n * 1.5f;
      break;
    case 3:
      punch_ = n;
      break;
    case 4:
      decay_ = 0.06f + n * n * 0.9f;
      break;
    case 5:
      sub_ = n;
      break;
    case 6:
      drive_ = n;
      break;
    case 7:
      level_ = 0.35f + n * 0.65f;
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t, int32_t) const override final { return nullptr; }
  void init(float *) override final { reset(); }

  void reset() override final {
    phase_ = 0.f;
    w_target_ = w_cur_ = 0.f;
    pitch_mul_ = 1.f;
    amp_ = 0.f;
    gate_ = false;
    slide_ = 0.035f;
    tail_ = 0.55f;
    tune_ = 1.f;
    punch_ = param_10bit_to_f32(614);
    decay_ = 0.35f;
    sub_ = param_10bit_to_f32(655);
    drive_ = param_10bit_to_f32(450);
    level_ = 0.9f;
  }

  void setPitch(float w0) { w_target_ = w0 * tune_; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void noteOn(uint8_t, uint8_t velo) override final {
    gate_ = true;
    if (amp_ < 0.01f) {
      pitch_mul_ = 1.8f + punch_ * 2.2f;
      phase_ = 0.f;
    }
    amp_ = fmaxf(amp_, static_cast<float>(velo) / 127.f);
  }

  void noteOff(uint8_t) override final { gate_ = false; }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    static constexpr float k_fs = 48000.f;
    (void)in;
    const float slide_c = slide_;
    const float decay_c = expf(-6.f / (decay_ * k_fs));
    const float tail_c = expf(-6.f / (tail_ * k_fs));

    for (const float *out_end = out + frames; out != out_end; ++out) {
      w_cur_ += (w_target_ - w_cur_) * slide_c;
      pitch_mul_ += (1.f - pitch_mul_) * (1.f / (decay_ * k_fs * 0.12f));

      if (gate_) {
        amp_ = fminf(1.f, amp_ + 0.02f);
      } else {
        amp_ *= tail_c;
      }
      amp_ *= decay_c * 0.9995f + 0.0005f;

      phase_ += w_cur_ * pitch_mul_;
      if (phase_ >= 1.f) {
        phase_ -= floorf(phase_);
      }

      float s = sinf(6.28318530718f * phase_);
      s += sinf(6.28318530718f * phase_ * 0.5f) * sub_ * 0.6f;
      s = tanhf(s * amp_ * (1.f + drive_ * 3.f) * level_);
      *out = s;
    }
  }

private:
  float phase_;
  float w_target_, w_cur_, pitch_mul_, amp_;
  bool gate_;
  float slide_, tail_, tune_, punch_, decay_, sub_, drive_, level_;
};
