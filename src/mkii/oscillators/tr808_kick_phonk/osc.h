#pragma once
/*
 * Phonk 808 kick — NTS-1 mkII port of src/oscillators/tr808-kick/kick808.cc
 */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum {
    WAVE = 0U,
    ATTACK = 1U,
    DECAY = 2U,
    RELEASE = 3U,
    FILTER = 4U,
    LP_TYPE = 5U,
    PITCH_SWP = 6U,
    SUB = 7U,
    DRIVE = 8U,
  };

  void setParameter(uint8_t index, int32_t value) override final {
    const float norm = param_10bit_to_f32(value);
    switch (index) {
    case WAVE:
      wave_mix_ = norm;
      break;
    case ATTACK:
      attack_sec_ = 0.0002f + norm * norm * 0.08f;
      break;
    case DECAY:
      decay_sec_ = 0.08f + norm * norm * norm * 9.5f;
      break;
    case RELEASE:
      release_sec_ = 0.04f + norm * norm * 5.0f;
      break;
    case FILTER:
      filter_cutoff_hz_ = 22.0f + norm * norm * 9000.0f;
      break;
    case LP_TYPE:
      filter_type_ = (value > 2) ? 2U : static_cast<uint8_t>(value);
      break;
    case PITCH_SWP:
      pitch_sweep_depth_ = 1.0f + norm * norm * 72.0f;
      break;
    case SUB:
      sub_amount_ = norm;
      break;
    case DRIVE:
      drive_amount_ = norm;
      break;
    default:
      break;
    }
  }

  void init(float *) override final { reset(); }

  void reset() override final {
    phase_ = 0U;
    env_stage_ = 0;
    amp_ = 0.0f;
    pitch_mul_ = 1.0f;
    w0_ = 0.0f;
    lp_z1_ = lp_z2_ = sub_z_ = 0.0f;
    wave_mix_ = param_10bit_to_f32(82);
    attack_sec_ = 0.0002f + 0.02f * 0.02f * 0.08f;
    decay_sec_ = 0.08f + 0.72f * 0.72f * 0.72f * 9.5f;
    release_sec_ = 0.04f + 0.38f * 0.38f * 5.0f;
    filter_cutoff_hz_ = 22.0f + 0.28f * 0.28f * 9000.0f;
    filter_type_ = 1U;
    pitch_sweep_depth_ = 1.0f + 0.78f * 0.78f * 72.0f;
    sub_amount_ = param_10bit_to_f32(634);
    drive_amount_ = param_10bit_to_f32(491);
  }

  void setPitch(float w0) { w0_ = w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void noteOn(uint8_t note, uint8_t velo) override final {
    (void)note;
    (void)velo;
    trigger();
  }

  void noteOff(uint8_t note) override final {
    (void)note;
    if (env_stage_ == 1 || env_stage_ == 2) {
      env_stage_ = 3;
    }
  }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    static constexpr float k_fs = 48000.0f;
    static constexpr float k_two_pi = 6.28318530718f;
    static constexpr float k_amp_floor = 0.00004f;

    const float pitch_decay_coef =
        1.0f - expf(-1.0f / (decay_sec_ * k_fs * 0.22f));
    const float attack_step =
        (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * k_fs)) : 1.0f;
    const float decay_coef = ampDecayCoef(decay_sec_);
    const float release_coef = ampDecayCoef(release_sec_);

    for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
      if (env_stage_ == 1) {
        amp_ += attack_step;
        if (amp_ >= 1.0f) {
          amp_ = 1.0f;
          env_stage_ = 2;
        }
      } else if (env_stage_ == 2) {
        amp_ *= decay_coef;
        if (amp_ < k_amp_floor) {
          amp_ = 0.0f;
          env_stage_ = 0;
        }
      } else if (env_stage_ == 3) {
        amp_ *= release_coef;
        if (amp_ < k_amp_floor) {
          amp_ = 0.0f;
          env_stage_ = 0;
        }
      }

      if (env_stage_ != 0) {
        pitch_mul_ += (1.0f - pitch_mul_) * pitch_decay_coef;
      }

      float sig = 0.0f;
      if (env_stage_ != 0 && amp_ > k_amp_floor) {
        const float w_scaled = w0_ * pitch_mul_;
        const uint32_t w_inc = static_cast<uint32_t>(w_scaled * 4294967296.0f);
        sig = oscSample(phase_, wave_mix_);
        sig *= amp_;
        sig = filterProcess(sig);
        sig = subBoost(sig);
        sig = phonkDrive(sig);
        phase_ += w_inc;
      }

      out[0] = sig;
    }
  }

private:
  static float ampDecayCoef(float seconds) {
    if (seconds < 0.01f) {
      seconds = 0.01f;
    }
    return expf(-6.90775527898f / (seconds * 48000.0f));
  }

  static float onePoleG(float cutoff_hz) {
    if (cutoff_hz < 20.0f) {
      cutoff_hz = 20.0f;
    }
    const float x = -6.28318530718f * cutoff_hz / 48000.0f;
    float g = 1.0f - expf(x);
    if (g < 0.0001f) {
      g = 0.0001f;
    }
    if (g > 1.0f) {
      g = 1.0f;
    }
    return g;
  }

  static float oscSample(uint32_t phase, float wave_mix) {
    const float norm = static_cast<float>(phase) * (1.0f / 4294967296.0f);
    const float angle = 6.28318530718f * norm;
    const float sine = sinf(angle);
    const float tri = 1.0f - 4.0f * fabsf(norm - 0.5f);
    return sine + (tri - sine) * wave_mix;
  }

  void trigger() {
    env_stage_ = 1;
    amp_ = 0.0f;
    pitch_mul_ = pitch_sweep_depth_;
    phase_ = 0U;
    lp_z1_ = lp_z2_ = sub_z_ = 0.0f;
  }

  float filterProcess(float in) {
    const float g = onePoleG(filter_cutoff_hz_);
    if (filter_type_ == 0U) {
      lp_z1_ += g * (in - lp_z1_);
      return lp_z1_;
    }
    if (filter_type_ == 1U) {
      lp_z1_ += g * (in - lp_z1_);
      lp_z2_ += g * (lp_z1_ - lp_z2_);
      return lp_z2_;
    }
    const float res = 0.42f;
    const float fb = lp_z2_ * res;
    const float x = in - fb;
    lp_z1_ += g * (x - lp_z1_);
    lp_z2_ += g * (lp_z1_ - lp_z2_);
    return lp_z2_;
  }

  float subBoost(float in) {
    const float g = onePoleG(58.0f);
    sub_z_ += g * (in - sub_z_);
    return in + sub_z_ * sub_amount_ * 2.2f;
  }

  float phonkDrive(float in) {
    const float gain = 1.0f + drive_amount_ * 5.5f;
    float x = in * gain;
    if (x > 1.0f) {
      x = 1.0f;
    }
    if (x < -1.0f) {
      x = -1.0f;
    }
    return tanhf(x * (1.0f + drive_amount_ * 0.85f));
  }

  uint32_t phase_;
  uint8_t env_stage_;
  float amp_;
  float pitch_mul_;
  float w0_;

  float wave_mix_;
  float attack_sec_;
  float decay_sec_;
  float release_sec_;
  float filter_cutoff_hz_;
  uint8_t filter_type_;
  float pitch_sweep_depth_;
  float sub_amount_;
  float drive_amount_;

  float lp_z1_;
  float lp_z2_;
  float sub_z_;
};
