#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/* Juno-106 DCO + chorus (no full bass LP/ADR rig). Ported from juno-rnb chorus/saw paths. */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  static constexpr uint32_t k_ch_len = 1536U;

  uint32_t getBufferSize() const override final { return 0; }

  enum {
    VIBE = 0U,
    SAW = 1U,
    PWM = 2U,
    CHORUS = 3U,
    GLIDE = 4U,
    ATTACK = 5U,
    DECAY = 6U,
    RELEASE = 7U,
  };

  void setParameter(uint8_t index, int32_t value) override final {
    const float n = param_10bit_to_f32(value);
    switch (index) {
    case VIBE:
      vibe_ = n;
      break;
    case SAW:
      saw_mix_ = n;
      break;
    case PWM:
      pwm_depth_ = n;
      break;
    case CHORUS:
      chorus_amt_ = n;
      break;
    case GLIDE:
      glide_ = n;
      break;
    case ATTACK:
      attack_sec_ = 0.001f + n * n * 0.35f;
      break;
    case DECAY:
      decay_sec_ = 0.03f + n * n * 1.2f;
      break;
    case RELEASE:
      release_sec_ = 0.04f + n * n * 1.5f;
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
    phase_ = 0.f;
    w0_ = w_cur_ = 0.f;
    amp_ = 0.f;
    env_stage_ = 0;
    gate_ = false;
    ch_wp_ = 0;
    lfo_phase_ = 0.f;
    for (uint32_t i = 0; i < k_ch_len; ++i) {
      ch_buf_[i] = 0.f;
    }
    vibe_ = param_10bit_to_f32(368);
    saw_mix_ = param_10bit_to_f32(737);
    pwm_depth_ = param_10bit_to_f32(286);
    chorus_amt_ = param_10bit_to_f32(450);
    glide_ = param_10bit_to_f32(245);
    attack_sec_ = 0.008f;
    decay_sec_ = 0.35f;
    release_sec_ = 0.4f;
    sustain_lvl_ = 0.78f;
  }

  void setPitch(float w0) { w0_ = w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void noteOn(uint8_t, uint8_t) override final {
    gate_ = true;
    if (glide_ < 0.02f || w_cur_ < 1e-9f) {
      w_cur_ = w0_;
    }
    if (env_stage_ == 0 || env_stage_ == 4) {
      env_stage_ = 1;
    }
  }

  void noteOff(uint8_t) override final {
    gate_ = false;
    if (env_stage_ != 0) {
      env_stage_ = 4;
    }
  }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    static constexpr float k_fs = 48000.f;
    static constexpr float k_two_pi = 6.28318530718f;
    (void)in;

    const float glide_coef = 0.00012f + glide_ * glide_ * 0.005f;
    const float attack_step = 1.f / (attack_sec_ * k_fs);
    const float decay_step = 1.f / (decay_sec_ * k_fs);
    const float release_step = 1.f / (release_sec_ * k_fs);
    const float lfo_inc = 0.8f / k_fs;

    for (const float *out_end = out + frames; out != out_end; ++out) {
      w_cur_ += (w0_ - w_cur_) * glide_coef;
      lfo_phase_ += lfo_inc;
      if (lfo_phase_ >= 1.f) {
        lfo_phase_ -= 1.f;
      }

      if (env_stage_ == 1) {
        amp_ += attack_step;
        if (amp_ >= 1.f) {
          amp_ = 1.f;
          env_stage_ = 2;
        }
      } else if (env_stage_ == 2) {
        amp_ += (sustain_lvl_ - amp_) * decay_step;
        if (fabsf(amp_ - sustain_lvl_) < 0.002f) {
          amp_ = sustain_lvl_;
          env_stage_ = 3;
        }
      } else if (env_stage_ == 3) {
        amp_ = sustain_lvl_;
        if (!gate_) {
          env_stage_ = 4;
        }
      } else if (env_stage_ == 4) {
        amp_ -= release_step;
        if (amp_ < 0.00006f) {
          amp_ = 0.f;
          env_stage_ = 0;
        }
      }

      float sig = 0.f;
      if (env_stage_ != 0) {
        phase_ += w_cur_;
        if (phase_ >= 1.f) {
          phase_ -= floorf(phase_);
        }
        const float lfo = sinf(k_two_pi * lfo_phase_);
        const float width = 0.5f + pwm_depth_ * 0.35f + lfo * pwm_depth_ * 0.08f;
        const float saw = 2.f * phase_ - 1.f;
        const float pulse = (phase_ < width) ? 1.f : -1.f;
        sig = saw * saw_mix_ + pulse * (1.f - saw_mix_ * 0.5f);
        sig = chorusProcess(sig);
        sig *= amp_ * (0.85f + vibe_ * 0.12f);
      }
      *out = sig;
    }
  }

private:
  float chorusProcess(float dry) {
    if (chorus_amt_ < 0.01f) {
      return dry;
    }
    const float lfo = sinf(6.28318530718f * lfo_phase_);
    const float base = 280.f + chorus_amt_ * 220.f;
    const float mod = 40.f + chorus_amt_ * 90.f;
    int delay = static_cast<int>(base + lfo * mod);
    if (delay < 24) {
      delay = 24;
    }
    if (delay >= static_cast<int>(k_ch_len) - 1) {
      delay = static_cast<int>(k_ch_len) - 2;
    }
    const uint32_t read = (ch_wp_ + k_ch_len - static_cast<uint32_t>(delay)) % k_ch_len;
    const float wet = ch_buf_[read];
    ch_buf_[ch_wp_] = dry;
    ch_wp_ = (ch_wp_ + 1U) % k_ch_len;
    const float mix = chorus_amt_ * (0.28f + vibe_ * 0.22f);
    return dry * (1.f - mix) + (dry + wet) * 0.5f * mix;
  }

  float ch_buf_[k_ch_len];
  uint32_t ch_wp_;
  float phase_;
  float lfo_phase_;
  float w0_;
  float w_cur_;
  float amp_;
  uint8_t env_stage_;
  bool gate_;
  float sustain_lvl_;

  float vibe_;
  float saw_mix_;
  float pwm_depth_;
  float chorus_amt_;
  float glide_;
  float attack_sec_;
  float decay_sec_;
  float release_sec_;
};
