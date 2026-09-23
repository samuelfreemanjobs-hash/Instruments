#pragma once
/* PORT_COMPLETE — Reese / detuned sub (3-voice supersaw core). */
#include "processor.h"
#include "supersaw_core.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  void setParameter(uint8_t index, int32_t value) override final {
    const float n = param_10bit_to_f32(value);
    switch (index) {
    case 0:
      detune_ = n;
      break;
    case 1:
      spread_ = n;
      break;
    case 2:
      sub_ = n;
      break;
    case 3:
      tone_ = 60.f + n * n * 800.f;
      break;
    case 4:
      level_ = 0.2f + n * 0.8f;
      break;
    case 5:
      glide_ = n;
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t, int32_t) const override final { return nullptr; }
  void init(float *) override final { reset(); }

  void reset() override final {
    ss_.resetPhases();
    w0_ = w_cur_ = 0.f;
    amp_ = 0.f;
    env_stage_ = 0;
    gate_ = false;
    tune_ = supersaw_tuning_rage();
    tune_.detune_cents_max = 55.f;
    tune_.sub_mix_default = 0.72f;
    detune_ = param_10bit_to_f32(798);
    spread_ = param_10bit_to_f32(737);
    sub_ = param_10bit_to_f32(819);
    tone_ = 120.f;
    level_ = 0.88f;
    glide_ = param_10bit_to_f32(286);
  }

  void setPitch(float w0) { w0_ = w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void noteOn(uint8_t, uint8_t) override final {
    gate_ = true;
    if (glide_ < 0.02f) {
      w_cur_ = w0_;
    }
    env_stage_ = 1;
    amp_ = 0.f;
  }

  void noteOff(uint8_t) override final {
    gate_ = false;
    env_stage_ = 2;
  }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    (void)in;
    const float glide_coef = 0.0002f + glide_ * glide_ * 0.008f;
    const int voices = 3;
    const float spread = (0.5f + detune_ * 0.5f) * (0.4f + spread_ * 0.6f);

    for (const float *out_end = out + frames; out != out_end; ++out) {
      w_cur_ += (w0_ - w_cur_) * glide_coef;
      if (env_stage_ == 1) {
        amp_ += 0.002f;
        if (amp_ >= 1.f) {
          amp_ = 1.f;
          env_stage_ = gate_ ? 3 : 2;
        }
      } else if (env_stage_ == 2) {
        amp_ *= 0.9995f;
        if (amp_ < 0.0001f) {
          amp_ = 0.f;
          env_stage_ = 0;
        }
      } else if (env_stage_ == 3 && !gate_) {
        env_stage_ = 2;
      }

      float sig = 0.f;
      if (env_stage_ != 0) {
        sig = ss_.processSample(w_cur_, voices, spread, sub_ * tune_.sub_mix_default, tone_, amp_,
                                tune_);
        sig *= level_;
      }
      *out = sig;
    }
  }

private:
  SupersawState ss_;
  SupersawTuning tune_;
  float w0_, w_cur_, amp_;
  uint8_t env_stage_;
  bool gate_;
  float detune_, spread_, sub_, tone_, level_, glide_;
};
