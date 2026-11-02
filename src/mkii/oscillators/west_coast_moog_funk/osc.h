#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * West Coast / Minimoog funk bass — mkII port of src/oscillators/west-coast-moog/funk_moog.cpp
 */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum {
    VIBE = 0U,
    DETUNE = 1U,
    FM = 2U,
    FOLD = 3U,
    GLIDE = 4U,
    ATTACK = 5U,
    DECAY = 6U,
    RELEASE = 7U,
    SUB = 8U,
  };

  void setParameter(uint8_t index, int32_t value) override final {
    const float norm = param_10bit_to_f32(value);
    switch (index) {
    case VIBE:
      vibe_ = norm;
      updateSustainLevel();
      break;
    case DETUNE:
      detune_ = norm;
      break;
    case FM:
      fm_depth_ = norm;
      break;
    case FOLD:
      fold_ = norm;
      break;
    case GLIDE:
      glide_ = norm;
      break;
    case ATTACK:
      attack_sec_ = timeSec(norm, 0.001f, 0.45f);
      break;
    case DECAY:
      decay_sec_ = timeSec(norm, 0.02f, 1.8f);
      break;
    case RELEASE:
      release_sec_ = timeSec(norm, 0.03f, 2.2f);
      break;
    case SUB:
      sub_mix_ = norm;
      break;
    default:
      break;
    }
  }

  void init(float *) override final { reset(); }

  void reset() override final {
    phase_a_ = phase_b_ = phase_sub_ = phase_fm_ = 0U;
    w_cur_ = 0.0f;
    env_stage_ = 0;
    gate_ = 0U;
    amp_ = 0.0f;
    filt_env_ = 0.0f;
    lp1_ = lp2_ = 0.0f;
    w0_ = 0.0f;
    host_cut_ = 1.0f;
    host_res_ = 0.5f;
    vibe_ = param_10bit_to_f32(225);
    detune_ = param_10bit_to_f32(389);
    fm_depth_ = param_10bit_to_f32(184);
    fold_ = param_10bit_to_f32(246);
    glide_ = param_10bit_to_f32(327);
    attack_sec_ = timeSec(param_10bit_to_f32(123), 0.001f, 0.45f);
    decay_sec_ = timeSec(param_10bit_to_f32(430), 0.02f, 1.8f);
    release_sec_ = timeSec(param_10bit_to_f32(358), 0.03f, 2.2f);
    sub_mix_ = param_10bit_to_f32(491);
    updateSustainLevel();
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

    const float glide_coef = 0.00015f + glide_ * glide_ * 0.012f;
    const float attack_step =
        (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * k_fs)) : 1.0f;
    const float decay_step =
        (decay_sec_ > 0.0001f) ? (1.0f / (decay_sec_ * k_fs)) : 1.0f;
    const float release_step =
        (release_sec_ > 0.0001f) ? (1.0f / (release_sec_ * k_fs)) : 1.0f;

    const float host_cut = host_cut_;
    const float host_res = host_res_;
    const float west = fminf(fmaxf((vibe_ - 0.52f) * 2.1f, 0.0f), 1.0f);
    const float detune_ratio = 1.0f + detune_ * (0.004f + west * 0.008f);
    const float pulse_w = 0.22f + (1.0f - vibe_) * 0.28f;

    for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
      w_cur_ += (w_tgt_ - w_cur_) * glide_coef;
      const uint32_t w = floatToW(w_cur_);
      const uint32_t w_b = floatToW(w_cur_ * detune_ratio);
      const uint32_t w_sub = w >> 1;
      const uint32_t w_fm = floatToW(w_cur_ * (2.0f + west * 2.5f));

      if (env_stage_ == 1) {
        amp_ += attack_step;
        filt_env_ += attack_step * 1.35f;
        if (amp_ >= 1.0f) {
          amp_ = 1.0f;
          filt_env_ = fminf(filt_env_, 1.0f);
          env_stage_ = 2;
        }
      } else if (env_stage_ == 2) {
        amp_ += (sustain_lvl_ - amp_) * decay_step;
        filt_env_ += (sustain_lvl_ * 0.65f - filt_env_) * decay_step;
        if (fabsf(amp_ - sustain_lvl_) < 0.002f) {
          amp_ = sustain_lvl_;
          env_stage_ = 3;
        }
      } else if (env_stage_ == 3) {
        if (gate_ == 0U) {
          env_stage_ = 4;
        }
        amp_ = sustain_lvl_;
        filt_env_ = sustain_lvl_ * 0.65f;
      } else if (env_stage_ == 4) {
        amp_ -= release_step;
        filt_env_ -= release_step * 0.9f;
        if (amp_ < k_amp_floor) {
          amp_ = 0.0f;
          filt_env_ = 0.0f;
          env_stage_ = 0;
        }
      }

      float sig = 0.0f;
      if (env_stage_ != 0) {
        const float fm = sinPh(phase_fm_) * fm_depth_ * (0.35f + west * 1.1f);
        const uint32_t ph_a = phase_a_ + (uint32_t)(fm * 1200000.0f);
        const float saw_a = saw(ph_a);
        const float saw_b = saw(phase_b_);
        const float pulse = pulseW(phase_b_, pulse_w);
        const float sub = saw(phase_sub_);

        sig = voiceMix(saw_a, saw_b, pulse, fm, west);
        sig += sub * sub_mix_ * 0.45f;

        const float base_hz = 90.0f + host_cut * (220.0f + (1.0f - vibe_) * 6800.0f);
        const float env_hz = base_hz + filt_env_ * (900.0f + vibe_ * 5200.0f);
        sig = moogLp(sig, env_hz, host_res * (0.35f + (1.0f - west) * 0.55f));
        sig *= amp_;
      }

      phase_a_ += w;
      phase_b_ += w_b;
      phase_sub_ += w_sub;
      phase_fm_ += w_fm;

      out[0] = sig;
    }
  }

private:
  static constexpr float k_fs = 48000.0f;
  static constexpr float k_two_pi = 6.28318530718f;
  static constexpr float k_amp_floor = 0.00008f;
  static constexpr float k_w_scale = 4294967296.0f;

  static float timeSec(float norm, float min_s, float max_s) {
    return min_s + norm * norm * max_s;
  }

  static uint32_t floatToW(float w) {
    if (w < 0.0f) {
      w = 0.0f;
    }
    if (w > 4294967295.0f) {
      w = 4294967295.0f;
    }
    return static_cast<uint32_t>(w);
  }

  static float saw(uint32_t phase) {
    return static_cast<float>(static_cast<int32_t>(phase >> 1)) * (1.0f / 2147483648.0f);
  }

  static float pulseW(uint32_t phase, float width) {
    const uint32_t threshold = static_cast<uint32_t>(width * 4294967295.0f);
    return (phase < threshold) ? 1.0f : -1.0f;
  }

  static float sinPh(uint32_t phase) {
    const float norm = static_cast<float>(phase) * (1.0f / 4294967296.0f);
    return sinf(k_two_pi * norm);
  }

  static float wavefold(float x, float amount) {
    if (amount < 0.001f) {
      return x;
    }
    const float gain = 1.0f + amount * 6.5f;
    x *= gain;
    const float folded = sinf(x * (1.2f + amount * 2.0f));
    return x * (1.0f - amount * 0.55f) + folded * (amount * 0.85f);
  }

  float onePoleG(float hz) const {
    if (hz < 40.0f) {
      hz = 40.0f;
    }
    if (hz > 16000.0f) {
      hz = 16000.0f;
    }
    return 1.0f - expf(-k_two_pi * hz / k_fs);
  }

  float moogLp(float in, float cutoff_hz, float res_norm) {
    const float g = onePoleG(cutoff_hz);
    const float k = 1.0f + res_norm * 3.2f;
    const float fb = lp2_ * (0.15f + res_norm * 0.55f);
    const float x = in - fb * k;
    lp1_ += g * (x - lp1_);
    lp2_ += g * (lp1_ - lp2_);
    return lp2_;
  }

  void updateSustainLevel() {
    const float bass = 1.0f - fminf(vibe_ * 1.35f, 1.0f);
    const float lead = fminf(fmaxf((vibe_ - 0.28f) * 1.55f, 0.0f), 1.0f);
    sustain_lvl_ = 0.42f * bass + 0.78f * lead + 0.25f * vibe_ * (1.0f - bass);
    if (sustain_lvl_ > 0.92f) {
      sustain_lvl_ = 0.92f;
    }
  }

  float voiceMix(float saw_a, float saw_b, float pulse, float fm_sin, float west) const {
    const float moog = 1.0f - west * 0.65f;
    const float bass = 1.0f - fminf(vibe_ * 1.2f, 1.0f);

    float core = (saw_a + saw_b) * (0.42f * moog + 0.18f);
    core += pulse * (0.35f * moog + 0.12f * bass);
    core += fm_sin * fm_depth_ * (0.25f + west * 1.4f);
    core += saw_a * sub_mix_ * (0.55f + bass * 0.35f);
    return wavefold(core, fold_ * (0.35f + west * 0.9f));
  }

  uint32_t phase_a_;
  uint32_t phase_b_;
  uint32_t phase_sub_;
  uint32_t phase_fm_;
  float w_cur_;
  float w_tgt_;
  float w0_;

  uint8_t env_stage_;
  uint8_t gate_;
  float amp_;
  float filt_env_;
  float sustain_lvl_;

  float vibe_;
  float detune_;
  float fm_depth_;
  float fold_;
  float glide_;
  float attack_sec_;
  float decay_sec_;
  float release_sec_;
  float sub_mix_;

  float host_cut_;
  float host_res_;

  float lp1_;
  float lp2_;
};
