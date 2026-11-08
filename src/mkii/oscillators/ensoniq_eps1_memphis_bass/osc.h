#pragma once
/* PORT_COMPLETE — do not overwrite with scaffold-mkii.py */
/*
 * mkII port of src/oscillators/ensoniq-eps1/eps_bass.cpp
 */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

#define EPS_WT_LEN 64U

static const int32_t k_eps_wt_sine[EPS_WT_LEN] = {
#include "eps_wt_sine.inc"
};

static const int32_t k_eps_wt_dark[EPS_WT_LEN] = {
#include "eps_wt_dark.inc"
};

static const int32_t k_eps_wt_grit[EPS_WT_LEN] = {
#include "eps_wt_grit.inc"
};

static inline int32_t dsp_lerp_q31(int32_t a, int32_t b, float t) {
  return static_cast<int32_t>((float)a + ((float)b - (float)a) * t);
}

class Osc : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0; }

  enum {
    VIBE = 0U,
    WAVE = 1U,
    DRIFT = 2U,
    DARK = 3U,
    RESO = 4U,
    GRIT = 5U,
    GLIDE = 6U,
    ATTACK = 7U,
    TAIL = 8U};

  static constexpr float k_fs = 48000.0f;
  static constexpr float k_two_pi = 6.28318530718f;
  static constexpr float k_w_scale = 4294967296.0f;
  static constexpr float k_amp_floor = 0.00008f;


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
    case WAVE:
      wave_ = norm;
      break;
    case DRIFT:
      drift_amt_ = norm;
      break;
    case DARK:
      dark_ = norm;
      break;
    case RESO:
      reso_ = norm;
      break;
    case GRIT:
      grit_ = norm;
      break;
    case GLIDE:
      glide_ = norm;
      break;
    case ATTACK:
      attack_sec_ = timeSec(norm, 0.002f, 0.22f);
      break;
    case TAIL:
      tail_sec_ = timeSec(norm, 0.05f, 1.8f);
      break;
    default:
      break;
    }
  }

  void init(float *) override final { reset(); }

  void reset() override final {
    phase_ = 0U;
    rng_ = 0xE951234U;
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
    setParameter(VIBE, 58);
    setParameter(WAVE, 40);
    setParameter(DRIFT, 52);
    setParameter(DARK, 68);
    setParameter(RESO, 45);
    setParameter(GRIT, 42);
    setParameter(GLIDE, 25);
    setParameter(ATTACK, 8);
    setParameter(TAIL, 55);
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
    phase_ = 0U;
    filt_env_ = 0.65f + reso_ * 0.25f;
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

  const float glide_coef = 0.0001f + glide_ * glide_ * 0.007f;
  const float attack_step =
      (attack_sec_ > 0.0001f) ? (1.0f / (attack_sec_ * k_fs)) : 1.0f;
  const float decay_step =
      (tail_sec_ > 0.0001f) ? (1.0f / (tail_sec_ * k_fs)) : 1.0f;
  const float release_step = decay_step * 0.9f;

  const float host_cut = host_cut_;
  const float host_res = host_res_;
  const uint32_t grit_hold_len = 1U + (uint32_t)(grit_ * 12.0f);

  for (const float *out_end = out + frames; out != out_end; in += 2, out += 1) {
    w_cur_ += (w_tgt_ - w_cur_) * glide_coef;

    if ((eps_rand() & 0x1FFFU) < 14U) {
      const float r = (float)(eps_rand() & 0xFFFFU) * (1.0f / 32768.0f) - 1.0f;
      drift_tgt_ = r * drift_amt_ * (0.006f + vibe_ * 0.01f);
    }
    drift_ += (drift_tgt_ - drift_) * 0.0004f;

    float w_f = w_cur_ * (1.0f + drift_);
    const uint32_t w = eps_float_to_w(w_f);

    if (env_stage_ == 1) {
      amp_ += attack_step;
      filt_env_ += attack_step * 0.8f;
      if (amp_ >= 1.0f) {
        amp_ = 1.0f;
        filt_env_ = fminf(filt_env_, 1.0f);
        env_stage_ = 2;
      }
    } else if (env_stage_ == 2) {
      amp_ += (sustain_lvl_ - amp_) * decay_step;
      filt_env_ += (sustain_lvl_ * 0.45f - filt_env_) * decay_step;
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
      filt_env_ -= release_step * 0.7f;
      if (amp_ < k_amp_floor) {
        amp_ = 0.0f;
        filt_env_ = 0.0f;
        env_stage_ = 0;
      }
    }

    float sig = 0.0f;
    if (env_stage_ != 0) {
      float morph = wave_ * 2.0f;
      int32_t wt;
      if (morph < 1.0f) {
        wt = eps_read_wt(k_eps_wt_sine, k_eps_wt_dark, phase_, morph);
      } else {
        morph = morph - 1.0f;
        if (morph > 1.0f) {
          morph = 1.0f;
        }
        wt = eps_read_wt(k_eps_wt_dark, k_eps_wt_grit, phase_, morph);
      }
      sig = (float)wt * (1.0f / 2147483648.0f);

      grit_ctr_++;
      if (grit_ctr_ >= grit_hold_len) {
        grit_ctr_ = 0U;
        grit_hold_ = sig;
      }
      sig = sig * (1.0f - grit_ * 0.55f) + grit_hold_ * (grit_ * 0.55f);

      float cut = 70.0f + host_cut * (90.0f + (1.0f - dark_) * 2200.0f);
      cut += filt_env_ * (200.0f + reso_ * 1400.0f);
      cut *= (1.0f - dark_ * 0.35f);
      const float res_use = reso_ * 0.65f + host_res * 0.35f;

      sig = eps_curtis_dark(sig, cut, res_use);
      sig *= amp_ * 0.9f;
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
  uint32_t rng_;
  float drift_;
  float drift_tgt_;
  float w_cur_;
  uint8_t env_stage_;
  uint8_t gate_;
  float amp_;
  float filt_env_;
  float vibe_;
  float wave_;
  float drift_amt_;
  float dark_;
  float reso_;
  float grit_;
  float glide_;
  float attack_sec_;
  float tail_sec_;
  float sustain_lvl_;
  float lp1_;
  float lp2_;
  float lp3_;
  float lp4_;
  float grit_hold_;
  uint32_t grit_ctr_;

  uint32_t eps_rand(void) {
    rng_ = rng_ * 1664525U + 1013904223U;
    return rng_;
  }

  uint32_t eps_float_to_w(float w) {
    if (w < 0.0f) {
      w = 0.0f;
    }
    if (w > 4294967295.0f) {
      w = 4294967295.0f;
    }
    return (uint32_t)w;
  }

  int32_t eps_read_wt(const int32_t *a, const int32_t *b, uint32_t phase, float t) {
    const uint32_t idx = phase >> 26;
    const uint32_t idx_n = (idx + 1U) & (EPS_WT_LEN - 1U);
    const float frac = (float)(phase & 0x3FFFFFFU) / (float)0x4000000U;
    return dsp_lerp_q31(a[idx], a[idx_n], frac) * (1.0f - t) +
           dsp_lerp_q31(b[idx], b[idx_n], frac) * t;
  }

  float eps_one_pole_g(float hz) {
    if (hz < 28.0f) {
      hz = 28.0f;
    }
    if (hz > 12000.0f) {
      hz = 12000.0f;
    }
    return 1.0f - expf(-k_two_pi * hz / k_fs);
  }

  float eps_curtis_dark(float in, float cutoff_hz, float res_norm) {
    const float g = eps_one_pole_g(cutoff_hz);
    const float res = res_norm * (0.35f + vibe_ * 0.4f);
    const float fb = lp4_ * (0.12f + res * 0.88f);
    float x = in - fb;

    lp1_ += g * (x - lp1_);
    lp2_ += g * (lp1_ - lp2_);
    lp3_ += g * (lp2_ - lp3_);
    lp4_ += g * (lp3_ - lp4_);
    return lp4_;
  }

  void updateSustain() {
    const float eerie = fminf(fmaxf((vibe_ - 0.35f) * 1.5f, 0.0f), 1.0f);
    sustain_lvl_ = 0.62f + eerie * 0.22f;
    if (sustain_lvl_ > 0.9f) {
      sustain_lvl_ = 0.9f;
    }
  }
};
