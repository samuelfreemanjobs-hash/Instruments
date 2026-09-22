#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * Dusty Memphis sub — mkII port inspired by memphis-juicy + extra lo-fi dust.
 */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum {
    VIBE = 0U,
    SUB = 1U,
    TRUNK = 2U,
    DUST = 3U,
    TAPE = 4U,
    MID = 5U,
    GLIDE = 6U,
    ATTACK = 7U,
    DECAY = 8U,
    RELEASE = 9U,
  };

  void setParameter(uint8_t index, int32_t value) override final {
    const float n = param_10bit_to_f32(value);
    switch (index) {
    case VIBE:
      vibe_ = n;
      updateSustain();
      break;
    case SUB:
      sub_lvl_ = n;
      break;
    case TRUNK:
      trunk_ = n;
      break;
    case DUST:
      dust_ = n;
      break;
    case TAPE:
      tape_ = n;
      break;
    case MID:
      mid_ = n;
      break;
    case GLIDE:
      glide_ = n;
      break;
    case ATTACK:
      attack_sec_ = timeSec(n, 0.001f, 0.14f);
      break;
    case DECAY:
      decay_sec_ = timeSec(n, 0.05f, 1.6f);
      break;
    case RELEASE:
      release_sec_ = timeSec(n, 0.05f, 1.5f);
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
    phase_deep_ = 0.f;
    w_cur_ = 0.f;
    env_stage_ = 0;
    gate_ = false;
    amp_ = 0.f;
    comp_env_ = 0.f;
    vibe_ = param_10bit_to_f32(542);
    sub_lvl_ = param_10bit_to_f32(798);
    trunk_ = param_10bit_to_f32(716);
    dust_ = param_10bit_to_f32(614);
    tape_ = param_10bit_to_f32(573);
    mid_ = param_10bit_to_f32(368);
    glide_ = param_10bit_to_f32(205);
    attack_sec_ = timeSec(param_10bit_to_f32(61), 0.001f, 0.14f);
    decay_sec_ = timeSec(param_10bit_to_f32(512), 0.05f, 1.6f);
    release_sec_ = timeSec(param_10bit_to_f32(491), 0.05f, 1.5f);
    sub_lp_ = mid_hp_z_ = tape_z_ = 0.f;
    dust_hold_ = 0.f;
    dust_counter_ = 0;
    rng_ = 0x9e3779b9U;
    updateSustain();
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
    if (env_stage_ == 0 || env_stage_ == 4) {
      env_stage_ = 1;
    }
  }

  void noteOff(uint8_t note) override final {
    (void)note;
    gate_ = false;
    if (env_stage_ != 0) {
      env_stage_ = 4;
    }
  }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    static constexpr float k_fs = 48000.0f;
    static constexpr float k_two_pi = 6.28318530718f;
    static constexpr float k_amp_floor = 0.00006f;

    const float glide_coef = 0.00012f + glide_ * glide_ * 0.005f;
    const float attack_step =
        (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * k_fs)) : 1.0f;
    const float decay_step =
        (decay_sec_ > 0.0001f) ? (1.0f / (decay_sec_ * k_fs)) : 1.0f;
    const float release_step =
        (release_sec_ > 0.0001f) ? (1.0f / (release_sec_ * k_fs)) : 1.0f;

    (void)in;
    const float cut_mix = 0.88f + vibe_ * 0.08f;

    for (const float *out_end = out + frames; out != out_end; out += 1) {
      w_cur_ += (w0_ - w_cur_) * glide_coef;

      if (env_stage_ == 1) {
        amp_ += attack_step;
        comp_env_ += attack_step * 1.1f;
        if (amp_ >= 1.0f) {
          amp_ = 1.0f;
          comp_env_ = fminf(comp_env_, 1.0f);
          env_stage_ = 2;
        }
      } else if (env_stage_ == 2) {
        amp_ += (sustain_lvl_ - amp_) * decay_step;
        comp_env_ += (sustain_lvl_ - comp_env_) * decay_step;
        if (fabsf(amp_ - sustain_lvl_) < 0.002f) {
          amp_ = sustain_lvl_;
          env_stage_ = 3;
        }
      } else if (env_stage_ == 3) {
        if (!gate_) {
          env_stage_ = 4;
        }
        amp_ = sustain_lvl_;
        comp_env_ = sustain_lvl_;
      } else if (env_stage_ == 4) {
        amp_ -= release_step;
        comp_env_ -= release_step;
        if (amp_ < k_amp_floor) {
          amp_ = 0.0f;
          comp_env_ = 0.0f;
          env_stage_ = 0;
        }
      }

      float sig = 0.f;
      if (env_stage_ != 0) {
        phase_ += w_cur_;
        if (phase_ >= 1.f) {
          phase_ -= floorf(phase_);
        }
        phase_deep_ += w_cur_ * 0.5f;
        if (phase_deep_ >= 1.f) {
          phase_deep_ -= floorf(phase_deep_);
        }

        const float sine = sinf(k_two_pi * phase_);
        const float deep = sinf(k_two_pi * phase_deep_);

        float sub = sine * sub_lvl_;
        sub += deep * trunk_ * (0.55f + sub_lvl_ * 0.25f);
        sub = subLp(sub);

        float mid = midBand(sine * comp_env_);
        sig = sub + mid;
        sig = tapeCompress(sig);
        sig = applyDust(sig);
        sig *= cut_mix;
        sig *= amp_ * (0.86f + trunk_ * 0.1f);
      }

      if (sig > 1.f) {
        sig = 1.f;
      }
      if (sig < -1.f) {
        sig = -1.f;
      }
      out[0] = sig;
    }
  }

private:
  static float timeSec(float norm, float min_s, float max_s) {
    return min_s + norm * norm * max_s;
  }

  void updateSustain() {
    sustain_lvl_ = 0.8f + vibe_ * 0.1f;
    if (sustain_lvl_ > 0.96f) {
      sustain_lvl_ = 0.96f;
    }
  }

  float onePoleG(float hz) const {
    if (hz < 20.f) {
      hz = 20.f;
    }
    if (hz > 12000.f) {
      hz = 12000.f;
    }
    return 1.f - expf(-6.28318530718f * hz / 48000.f);
  }

  float subLp(float in) {
    const float hz = 38.f + (1.f - trunk_) * 62.f;
    const float g = onePoleG(hz);
    sub_lp_ += g * (in - sub_lp_);
    return sub_lp_;
  }

  float midBand(float sine) {
    if (mid_ < 0.01f) {
      return 0.f;
    }
    float shaped = sine;
    const float drive = 1.f + mid_ * (2.2f + vibe_ * 2.5f);
    shaped = tanhf(shaped * drive);
    shaped = shaped * shaped * (mid_ * 0.38f);
    const float g = onePoleG(200.f + mid_ * 520.f);
    mid_hp_z_ += g * (shaped - mid_hp_z_);
    return mid_hp_z_ * mid_ * (0.2f + vibe_ * 0.2f);
  }

  float tapeCompress(float in) {
    if (tape_ < 0.01f) {
      return in;
    }
    const float thresh = 0.5f - tape_ * 0.22f;
    float x = in;
    if (x > thresh) {
      x = thresh + (x - thresh) / (1.f + tape_ * 4.5f);
    }
    if (x < -thresh) {
      x = -thresh + (x + thresh) / (1.f + tape_ * 4.5f);
    }
    tape_z_ += 0.0014f * (tanhf(x * (1.f + tape_ * 2.f)) - tape_z_);
    return in * (1.f - tape_ * 0.28f) + tape_z_ * (tape_ * 0.7f);
  }

  uint32_t rngStep() {
    rng_ ^= rng_ << 13;
    rng_ ^= rng_ >> 17;
    rng_ ^= rng_ << 5;
    return rng_;
  }

  float applyDust(float in) {
    if (dust_ < 0.01f) {
      return in;
    }
    const float noise = (static_cast<float>(rngStep() & 0xffffU) / 32768.f - 1.f) * dust_ * 0.08f;
    float x = in + noise * (0.4f + trunk_ * 0.3f);

    const int hold_len = static_cast<int>(4.f + (1.f - dust_) * 28.f);
    if (++dust_counter_ >= hold_len) {
      dust_counter_ = 0;
      dust_hold_ = x;
    }
    x = dust_hold_;

    const float steps = 8.f + (1.f - dust_) * 120.f;
    x = floorf(x * steps + 0.5f) / steps;
    return x * (0.92f + dust_ * 0.12f);
  }

  float phase_;
  float phase_deep_;
  float w0_;
  float w_cur_;
  uint8_t env_stage_;
  bool gate_;
  float amp_;
  float comp_env_;
  float sustain_lvl_;

  float vibe_;
  float sub_lvl_;
  float trunk_;
  float dust_;
  float tape_;
  float mid_;
  float glide_;
  float attack_sec_;
  float decay_sec_;
  float release_sec_;

  float sub_lp_;
  float mid_hp_z_;
  float tape_z_;
  float dust_hold_;
  int dust_counter_;
  uint32_t rng_;
};
