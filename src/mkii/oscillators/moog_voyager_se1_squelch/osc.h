#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * mkII port of src/oscillators/moog-voyager-se/voyager_se.cpp
 */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum {
    VIBE = 0U,
    FAT = 1U,
    SQUARE = 2U,
    SUB = 3U,
    DRIVE = 4U,
    GLIDE = 5U,
    SQUELCH = 6U,
    ATTACK = 7U,
    DECAY = 8U};

  static constexpr float k_fs = 48000.0f;
  static constexpr float k_two_pi = 6.28318530718f;
  static constexpr float k_w_scale = 4294967296.0f;
  static constexpr float k_amp_floor = 0.00008f;
  static constexpr auto VS_NUM_OSC = 3U;

  static float timeSec(float norm, float min_s, float max_s) {
    return min_s + norm * norm * max_s;
  }

  void setParameter(uint8_t index, int32_t value) override final {
    const float norm = param_10bit_to_f32(value);
    switch (index) {
    case VIBE:
      vibe_ = norm;
      updateSustain();
      break;
    case FAT:
      fat_ = norm;
      break;
    case SQUARE:
      square_ = norm;
      break;
    case SUB:
      sub_lvl_ = norm;
      break;
    case DRIVE:
      drive_ = norm;
      break;
    case GLIDE:
      glide_ = norm;
      break;
    case SQUELCH:
      squelch_ = norm;
      break;
    case ATTACK:
      attack_sec_ = timeSec(norm, 0.001f, 0.18f);
      break;
    case DECAY:
      decay_sec_ = timeSec(norm, 0.025f, 1.4f);
      break;
    default:
      break;
    }
  }

  void init(float *) override final { reset(); }

  void reset() override final {
    for (uint32_t i = 0; i < VS_NUM_OSC; i++) {
    phase_[i] = 0U;
    }
    phase_sub_ = 0U;
    w_cur_ = 0.0f;
    w_tgt_ = 0.0f;
    env_stage_ = 0;
    gate_ = 0U;
    amp_ = 0.0f;
    filt_env_ = 0.0f;
    lp1_ = 0.0f;
    lp2_ = 0.0f;
    lp3_ = 0.0f;
    lp4_ = 0.0f;
    updateSustain();
    w0_ = 0.0f;
    host_cut_ = 1.0f;
    host_res_ = 0.5f;
  }

  void setPitch(float w0) { w0_ = w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void setHostCutoffRes(float cut, float res) {
    host_cut_ = cut;
    host_res_ = res;
  }

  void noteOn(uint8_t note, uint8_t velo) override final {
    (void)note;
    (void)velo;
    gate_ = 1U;
    w_tgt_ = w0_ * k_w_scale;
    if (glide_ < 0.02f || w_cur_ < 1.0f) {
    w_cur_ = w_tgt_;
    }
    for (uint32_t i = 0; i < VS_NUM_OSC; i++) {
    phase_[i] = 0U;
    }
    phase_sub_ = 0U;
    filt_env_ = 0.85f + squelch_ * 0.15f;
    if (env_stage_ == 0 || env_stage_ == 4) {
    env_stage_ = 1;
    }
  }

  void noteOff(uint8_t note) override final {
    (void)note;
    gate_ = 0U;
    if (env_stage_ != 0) {
    env_stage_ = 4;
    }
  }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    (void)in;
    w_tgt_ = w0_ * k_w_scale;
w_tgt_ = w0_ * k_w_scale;

  const float glide_coef = 0.0001f + glide_ * glide_ * 0.014f;
  const float attack_step =
      (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * k_fs)) : 1.0f;
  const float decay_step =
      (decay_sec_ > 0.0001f) ? (1.0f / (decay_sec_ * k_fs)) : 1.0f;
  const float filt_decay = decay_step * (1.2f + squelch_ * 2.5f);
  const float release_step = filt_decay * 0.85f;

  const float host_cut = host_cut_;
  const float host_res = host_res_;

  for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
    w_cur_ += (w_tgt_ - w_cur_) * glide_coef;

    if (env_stage_ == 1) {
      amp_ += attack_step;
      filt_env_ += attack_step * (1.5f + squelch_ * 1.2f);
      if (amp_ >= 1.0f) {
        amp_ = 1.0f;
        if (filt_env_ > 1.0f) {
          filt_env_ = 1.0f;
        }
        env_stage_ = 2;
      }
    } else if (env_stage_ == 2) {
      amp_ += (sustain_lvl_ - amp_) * decay_step;
      filt_env_ -= filt_env_ * filt_decay;
      if (filt_env_ < sustain_lvl_ * (0.25f + squelch_ * 0.35f)) {
        filt_env_ = sustain_lvl_ * (0.25f + squelch_ * 0.35f);
      }
      if (fabsf(amp_ - sustain_lvl_) < 0.002f) {
        amp_ = sustain_lvl_;
        env_stage_ = 3;
      }
    } else if (env_stage_ == 3) {
      if (gate_ == 0U) {
        env_stage_ = 4;
      }
      amp_ = sustain_lvl_;
    } else if (env_stage_ == 4) {
      amp_ -= release_step;
      filt_env_ -= release_step * 1.1f;
      if (amp_ < k_amp_floor) {
        amp_ = 0.0f;
        filt_env_ = 0.0f;
        env_stage_ = 0;
      }
    }

    float sig = 0.0f;
    if (env_stage_ != 0) {
      sig = voyager_stack_sample();
      sig = voyager_moog_drive(sig);

      const float model_d = 1.0f - fminf(vibe_ * 0.8f, 1.0f);
      const float base_hz =
          55.0f + host_cut * (110.0f + model_d * 2800.0f + vibe_ * 6500.0f);
      const float squelch_hz =
          base_hz + filt_env_ * (600.0f + squelch_ * 6800.0f + vibe_ * 3200.0f);
      const float res_use = host_res * (0.35f + squelch_ * 0.55f);

      sig = voyager_ladder_squelch(sig, squelch_hz, res_use);
      sig *= amp_;
    }

    out[0] = sig;
  }
  }

private:
  float w0_;
  float w_tgt_;
  float host_cut_;
  float host_res_;

  uint32_t phase_sub_;
  float w_cur_;
  uint8_t env_stage_;
  uint8_t gate_;
  float amp_;
  float filt_env_;
  float vibe_;
  float fat_;
  float square_;
  float sub_lvl_;
  float drive_;
  float glide_;
  float squelch_;
  float attack_sec_;
  float decay_sec_;
  float sustain_lvl_;
  float lp1_;
  float lp2_;
  float lp3_;
  float lp4_;
  uint32_t phase_[3];

  float voyager_norm100(uint16_t value) {
    if (value > 100U) {
      value = 100U;
    }
    return (float)value * 0.01f;
  }

  float voyager_time_sec(uint16_t value, float min_s, float max_s) {
    const float n = voyager_norm100(value);
    return min_s + n * n * max_s;
  }

  uint32_t voyager_float_to_w(float w) {
    if (w < 0.0f) {
      w = 0.0f;
    }
    if (w > 4294967295.0f) {
      w = 4294967295.0f;
    }
    return (uint32_t)w;
  }

  float voyager_saw(uint32_t phase) {
    return (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
  }

  float voyager_square(uint32_t phase) {
    return (phase & 0x80000000U) ? 1.0f : -1.0f;
  }

  float voyager_one_pole_g(float hz) {
    if (hz < 40.0f) {
      hz = 40.0f;
    }
    if (hz > 18000.0f) {
      hz = 18000.0f;
    }
    return 1.0f - expf(-k_two_pi * hz / k_fs);
  }

  float voyager_moog_drive(float in) {
    const float amount = drive_ * (0.4f + vibe_ * 0.55f);
    if (amount < 0.01f) {
      return in;
    }
    const float g = 1.0f + amount * 4.5f;
    return tanhf(in * g) * (1.0f / (1.0f + amount * 0.28f));
  }

  float voyager_ladder_squelch(float in, float cutoff_hz, float res_norm) {
    const float g = voyager_one_pole_g(cutoff_hz);
    const float sq = squelch_ * (0.55f + vibe_ * 0.45f);
    float res = res_norm + sq * 0.55f;
    if (res > 1.15f) {
      res = 1.15f;
    }
    const float fb = lp4_ * (0.18f + res * 1.05f);
    float x = in - fb;

    lp1_ += g * (x - lp1_);
    lp2_ += g * (lp1_ - lp2_);
    lp3_ += g * (lp2_ - lp3_);
    lp4_ += g * (lp3_ - lp4_);
    return lp4_ * (1.0f + sq * 0.12f);
  }

  void updateSustain() {
    const float model_d = 1.0f - fminf(vibe_ * 0.85f, 1.0f);
    const float squelch_w = fminf(fmaxf((vibe_ - 0.45f) * 1.85f, 0.0f), 1.0f);
    sustain_lvl_ = 0.62f * model_d + 0.48f * squelch_w + 0.22f;
    if (sustain_lvl_ > 0.94f) {
      sustain_lvl_ = 0.94f;
    }
  }

  float voyager_stack_sample(void) {
    const float spread = fat_ * (0.004f + vibe_ * 0.006f);
    const float weights[3] = {0.38f, 0.34f, 0.28f};
    const float ratios[3] = {1.0f - spread, 1.0f, 1.0f + spread * 1.15f};

    float mix = 0.0f;
    for (uint32_t o = 0; o < 3; o++) {
      const uint32_t w_o = voyager_float_to_w(w_cur_ * ratios[o]);
      const float saw = voyager_saw(phase_[o]);
      const float sq = voyager_square(phase_[o]);
      mix += (saw * (1.0f - square_ * 0.85f) + sq * (square_ * 0.85f)) * weights[o];
      phase_[o] += w_o;
    }

    const uint32_t w_sub = voyager_float_to_w(w_cur_ * 0.5f);
    const float sub = voyager_square(phase_sub_) * sub_lvl_ * 0.5f;
    phase_sub_ += w_sub;

    return mix + sub;
  }
};
