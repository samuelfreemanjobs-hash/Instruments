#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * mkII port of src/oscillators/juno-rnb/juno_rnb.cpp
 */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
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
    HPF = 8U};

  static constexpr float k_fs = 48000.0f;
  static constexpr float k_two_pi = 6.28318530718f;
  static constexpr float k_w_scale = 4294967296.0f;
  static constexpr float k_amp_floor = 0.00008f;
  static constexpr auto JR_CHORUS_LEN = 1536U;

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
    case SAW:
      saw_mix_ = norm;
      break;
    case PWM:
      pwm_depth_ = norm;
      break;
    case CHORUS:
      chorus_amt_ = norm;
      break;
    case GLIDE:
      glide_ = norm;
      break;
    case ATTACK:
      attack_sec_ = timeSec(norm, 0.001f, 0.35f);
      break;
    case DECAY:
      decay_sec_ = timeSec(norm, 0.03f, 1.4f);
      break;
    case RELEASE:
      release_sec_ = timeSec(norm, 0.04f, 1.8f);
      break;
    case HPF:
      hpf_amt_ = norm;
      break;
    default:
      break;
    }
  }

  void init(float *) override final { reset(); }

  void reset() override final {
    phase_ = 0U;
    lfo_phase_ = 0U;
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
    hp_z_ = 0.0f;
    ch_wp_ = 0U;
    for (uint32_t i = 0; i < JR_CHORUS_LEN; i++) {
    ch_buf_[i] = 0.0f;
    }
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

  const float glide_coef = 0.00012f + glide_ * glide_ * 0.01f;
  const float attack_step =
      (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * k_fs)) : 1.0f;
  const float decay_step =
      (decay_sec_ > 0.0001f) ? (1.0f / (decay_sec_ * k_fs)) : 1.0f;
  const float release_step =
      (release_sec_ > 0.0001f) ? (1.0f / (release_sec_ * k_fs)) : 1.0f;

  const float host_cut = host_cut_;
  const float host_res = host_res_;
  const uint32_t lfo_inc = 220452492U;

  for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
    w_cur_ += (w_tgt_ - w_cur_) * glide_coef;
    const uint32_t w = juno_float_to_w(w_cur_);

    lfo_phase_ += lfo_inc;

    if (env_stage_ == 1) {
      amp_ += attack_step;
      filt_env_ += attack_step * 1.1f;
      if (amp_ >= 1.0f) {
        amp_ = 1.0f;
        filt_env_ = fminf(filt_env_, 1.0f);
        env_stage_ = 2;
      }
    } else if (env_stage_ == 2) {
      amp_ += (sustain_lvl_ - amp_) * decay_step;
      filt_env_ += (sustain_lvl_ * 0.55f - filt_env_) * decay_step;
      if (fabsf(amp_ - sustain_lvl_) < 0.002f) {
        amp_ = sustain_lvl_;
        env_stage_ = 3;
      }
    } else if (env_stage_ == 3) {
      if (gate_ == 0U) {
        env_stage_ = 4;
      }
      amp_ = sustain_lvl_;
      filt_env_ = sustain_lvl_ * 0.55f;
    } else if (env_stage_ == 4) {
      amp_ -= release_step;
      filt_env_ -= release_step * 0.85f;
      if (amp_ < k_amp_floor) {
        amp_ = 0.0f;
        filt_env_ = 0.0f;
        env_stage_ = 0;
      }
    }

    float sig = 0.0f;
    if (env_stage_ != 0) {
      const float pwm = 0.42f + juno_lfo_sin() * pwm_depth_ * 0.22f;
      const float saw = juno_saw(phase_);
      const float square = juno_pulse(phase_, pwm);
      sig = saw * saw_mix_ + square * (1.0f - saw_mix_);

      const float dark = 1.0f - fminf(vibe_ * 1.1f, 1.0f);
      const float base_hz =
          70.0f + host_cut * (120.0f + dark * 420.0f + vibe_ * 3800.0f);
      const float env_hz = base_hz + filt_env_ * (350.0f + vibe_ * 1800.0f);
      const float res_use = host_res * (0.25f + dark * 0.35f);

      sig = juno_hpf(sig);
      sig = juno_roland_lp(sig, env_hz, res_use);
      sig = juno_chorus(sig);
      sig *= amp_ * (0.88f + dark * 0.08f);
    }

    phase_ += w;
    out[0] = sig;
  }
  }

private:
  float w0_;
  float w_tgt_;
  float host_cut_;
  float host_res_;

  uint32_t phase_;
  uint32_t lfo_phase_;
  float w_cur_;
  uint8_t env_stage_;
  uint8_t gate_;
  float amp_;
  float filt_env_;
  float vibe_;
  float saw_mix_;
  float pwm_depth_;
  float chorus_amt_;
  float glide_;
  float attack_sec_;
  float decay_sec_;
  float release_sec_;
  float hpf_amt_;
  float sustain_lvl_;
  float lp1_;
  float lp2_;
  float lp3_;
  float lp4_;
  float hp_z_;
  uint32_t ch_wp_;
  float ch_buf_[1536];

  float juno_norm100(uint16_t value) {
    if (value > 100U) {
      value = 100U;
    }
    return (float)value * 0.01f;
  }

  float juno_time_sec(uint16_t value, float min_s, float max_s) {
    const float n = juno_norm100(value);
    return min_s + n * n * max_s;
  }

  uint32_t juno_float_to_w(float w) {
    if (w < 0.0f) {
      w = 0.0f;
    }
    if (w > 4294967295.0f) {
      w = 4294967295.0f;
    }
    return (uint32_t)w;
  }

  float juno_saw(uint32_t phase) {
    return (float)(int32_t)(phase >> 1) * (1.0f / 2147483648.0f);
  }

  float juno_pulse(uint32_t phase, float width) {
    if (width < 0.05f) {
      width = 0.05f;
    }
    if (width > 0.95f) {
      width = 0.95f;
    }
    const uint32_t threshold = (uint32_t)(width * 4294967295.0f);
    return (phase < threshold) ? 1.0f : -1.0f;
  }

  float juno_lfo_sin(void) {
    const float norm = (float)lfo_phase_ * (1.0f / 4294967296.0f);
    return sinf(k_two_pi * norm);
  }

  float juno_one_pole_g(float hz) {
    if (hz < 30.0f) {
      hz = 30.0f;
    }
    if (hz > 18000.0f) {
      hz = 18000.0f;
    }
    return 1.0f - expf(-k_two_pi * hz / k_fs);
  }

  float juno_hpf(float in) {
    if (hpf_amt_ < 0.01f) {
      return in;
    }
    const float cutoff = 25.0f + hpf_amt_ * hpf_amt_ * 320.0f;
    const float g = juno_one_pole_g(cutoff);
    hp_z_ += g * (in - hp_z_);
    const float hp = in - hp_z_;
    return hp * (0.35f + hpf_amt_ * 0.85f) + in * (1.0f - hpf_amt_ * 0.45f);
  }

  float juno_roland_lp(float in, float cutoff_hz, float res_norm) {
    const float g = juno_one_pole_g(cutoff_hz);
    const float fb = lp4_ * (0.08f + res_norm * 0.72f);
    float x = in - fb;

    lp1_ += g * (x - lp1_);
    lp2_ += g * (lp1_ - lp2_);
    lp3_ += g * (lp2_ - lp3_);
    lp4_ += g * (lp3_ - lp4_);
    return lp4_;
  }

  float juno_chorus(float dry) {
    if (chorus_amt_ < 0.01f) {
      return dry;
    }

    const float lfo = juno_lfo_sin();
    const float base = 280.0f + chorus_amt_ * 220.0f;
    const float mod = 40.0f + chorus_amt_ * 90.0f;
    int32_t delay = (int32_t)(base + lfo * mod);
    if (delay < 24) {
      delay = 24;
    }
    if (delay >= (int32_t)1536) {
      delay = (int32_t)1536 - 1;
    }

    const uint32_t read = (ch_wp_ + 1536 - (uint32_t)delay) % 1536;
    const float wet = ch_buf_[read];
    ch_buf_[ch_wp_] = dry;
    ch_wp_ = (ch_wp_ + 1U) % 1536;

    const float mix = chorus_amt_ * (0.28f + vibe_ * 0.22f);
    return dry * (1.0f - mix) + (dry + wet) * 0.5f * mix;
  }

  void updateSustain() {
    const float ballad = 1.0f - fminf(vibe_ * 1.25f, 1.0f);
    const float bright = fminf(fmaxf((vibe_ - 0.42f) * 1.7f, 0.0f), 1.0f);
    sustain_lvl_ = 0.58f * ballad + 0.82f * (1.0f - ballad) + bright * 0.12f;
    if (sustain_lvl_ > 0.95f) {
      sustain_lvl_ = 0.95f;
    }
  }
};
