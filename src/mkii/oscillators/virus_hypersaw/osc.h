#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * Access Virus–style hypersaw for NTS-1 mkII (shared supersaw_core.h).
 */
#include "processor.h"
#include "supersaw_core.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum {
    DETUNE = 0U,
    VOICES = 1U,
    WIDTH = 2U,
    SUB = 3U,
    TONE = 4U,
    LEVEL = 5U,
    ATTACK = 6U,
    RELEASE = 7U,
    GLIDE = 8U,
  };

  void setParameter(uint8_t index, int32_t value) override final {
    const float n = param_10bit_to_f32(value);
    switch (index) {
    case DETUNE:
      detune_ = n;
      break;
    case VOICES:
      voices_norm_ = n;
      break;
    case WIDTH:
      width_ = n;
      break;
    case SUB:
      sub_ = n;
      break;
    case TONE:
      tone_hp_ = 80.f + n * n * 3200.f;
      break;
    case LEVEL:
      level_ = 0.2f + n * 0.8f;
      break;
    case ATTACK:
      attack_sec_ = 0.002f + n * n * 0.35f;
      break;
    case RELEASE:
      release_sec_ = 0.02f + n * n * 1.8f;
      break;
    case GLIDE:
      glide_ = n;
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

  void init(float *) override final { reset(); }

  void reset() override final {
    ss_.resetPhases();
    w0_ = 0.f;
    w_cur_ = 0.f;
    amp_ = 0.f;
    env_stage_ = 0;
    gate_ = false;
    tune_ = supersaw_tuning_virus();
    detune_ = param_10bit_to_f32(716);
    voices_norm_ = param_10bit_to_f32(798);
    width_ = param_10bit_to_f32(614);
    sub_ = param_10bit_to_f32(491);
    tone_hp_ = tune_.hp_hz_default + 400.f;
    level_ = 0.85f;
    attack_sec_ = 0.008f;
    release_sec_ = 0.25f;
    glide_ = param_10bit_to_f32(205);
  }

  void setPitch(float w0) { w0_ = w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void noteOn(uint8_t note, uint8_t velo) override final {
    (void)note;
    (void)velo;
    gate_ = true;
    if (glide_ < 0.02f || w_cur_ < 1e-9f) {
      w_cur_ = w0_;
    }
    if (env_stage_ == 0 || env_stage_ == 2) {
      env_stage_ = 1;
    }
  }

  void noteOff(uint8_t note) override final {
    (void)note;
    gate_ = false;
    if (env_stage_ != 0) {
      env_stage_ = 2;
    }
  }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    static constexpr float k_fs = 48000.f;
    static constexpr float k_amp_floor = 0.00005f;

    (void)in;
    const int num_voices = 2 + static_cast<int>(voices_norm_ * 5.99f);
    const float spread = (0.35f + detune_ * 0.65f) * (0.5f + width_ * 0.5f);
    const float sub_amt = sub_ * tune_.sub_mix_default;
    const float glide_coef = 0.00015f + glide_ * glide_ * 0.006f;
    const float attack_step =
        (attack_sec_ > 0.0001f) ? (1.f / (attack_sec_ * k_fs)) : 1.f;
    const float release_step =
        (release_sec_ > 0.0001f) ? (1.f / (release_sec_ * k_fs)) : 1.f;

    for (const float *out_end = out + frames; out != out_end; ++out) {
      w_cur_ += (w0_ - w_cur_) * glide_coef;

      if (env_stage_ == 1) {
        amp_ += attack_step;
        if (amp_ >= 1.f) {
          amp_ = 1.f;
          env_stage_ = gate_ ? 3 : 2;
        }
      } else if (env_stage_ == 2) {
        amp_ -= release_step;
        if (amp_ < k_amp_floor) {
          amp_ = 0.f;
          env_stage_ = 0;
        }
      } else if (env_stage_ == 3) {
        amp_ = 1.f;
        if (!gate_) {
          env_stage_ = 2;
        }
      }

      float sig = 0.f;
      if (env_stage_ != 0) {
        sig = ss_.processSample(w_cur_, num_voices, spread, sub_amt, tone_hp_, amp_, tune_);
        sig *= level_;
      }

      if (sig > 1.f) {
        sig = 1.f;
      }
      if (sig < -1.f) {
        sig = -1.f;
      }
      *out = sig;
    }
  }

private:
  SupersawState ss_;
  SupersawTuning tune_;
  float w0_;
  float w_cur_;
  float amp_;
  uint8_t env_stage_;
  bool gate_;

  float detune_;
  float voices_norm_;
  float width_;
  float sub_;
  float tone_hp_;
  float level_;
  float attack_sec_;
  float release_sec_;
  float glide_;
};
