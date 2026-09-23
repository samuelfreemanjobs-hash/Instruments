#pragma once
/* PORT_COMPLETE — parallel NY-style comp: dry + squashed branch. */
#include "processor.h"
#include "unit_modfx.h"

#include <math.h>

class ParallelCompFx : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0U; }

  enum {
    THRS = 0U,
    RAT = 1U,
    ATK = 2U,
    REL = 3U,
    PAR = 4U,
    DRV = 5U,
    MIX = 6U,
    TONE = 7U,
  };

  void setParameter(uint8_t index, int32_t value) override final {
    const float n = param_10bit_to_f32(value);
    switch (index) {
    case THRS:
      thresh_ = 0.05f + n * 0.55f;
      break;
    case RAT:
      ratio_ = 1.5f + n * n * 8.f;
      break;
    case ATK:
      atk_coef_ = expf(-1.f / (0.0005f + n * 0.04f * 48000.f));
      break;
    case REL:
      rel_coef_ = expf(-1.f / (0.01f + n * n * 0.35f * 48000.f));
      break;
    case PAR:
      parallel_ = n;
      break;
    case DRV:
      drive_ = 1.f + n * 3.f;
      break;
    case MIX:
      mix_ = (value / 1000.f + 1.f) * 0.5f;
      break;
    case TONE:
      tone_ = n;
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t, int32_t) const override final { return nullptr; }

  void init(float *) override final { reset(); }
  void teardown() override final {}

  void reset() override final {
    env_l_ = env_r_ = 0.f;
    lp_l_ = lp_r_ = 0.f;
    thresh_ = 0.22f;
    ratio_ = 4.f;
    atk_coef_ = expf(-1.f / (0.003f * 48000.f));
    rel_coef_ = expf(-1.f / (0.12f * 48000.f));
    parallel_ = param_10bit_to_f32(614);
    drive_ = 1.8f;
    mix_ = 0.5f;
    tone_ = param_10bit_to_f32(512);
  }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    static constexpr float k_fs = 48000.f;
    const float dry_w = 1.f - mix_;
    const float wet_w = mix_;
    const float lp_g = 1.f - expf(-6.28318530718f * (800.f + tone_ * 6000.f) / k_fs);

    for (const float *out_end = out + frames * 2; out != out_end; in += 2, out += 2) {
      const float dl = in[0];
      const float dr = in[1];
      const float det = 0.5f * (fabsf(dl) + fabsf(dr));

      if (det > env_l_) {
        env_l_ = atk_coef_ * env_l_ + (1.f - atk_coef_) * det;
      } else {
        env_l_ = rel_coef_ * env_l_ + (1.f - rel_coef_) * det;
      }
      env_r_ = env_l_;

      float gr = 1.f;
      if (env_l_ > thresh_) {
        const float over = env_l_ - thresh_;
        gr = thresh_ + over / ratio_;
        gr = (gr > 0.0001f) ? (gr / env_l_) : 0.f;
      }

      float cl = dl * gr * drive_;
      float cr = dr * gr * drive_;
      cl = tanhf(cl);
      cr = tanhf(cr);

      lp_l_ += lp_g * (cl - lp_l_);
      lp_r_ += lp_g * (cr - lp_r_);
      cl = lp_l_;
      cr = lp_r_;

      const float blend = parallel_;
      float ol = dl * (1.f - blend) + cl * blend;
      float or = dr * (1.f - blend) + cr * blend;

      out[0] = dl * dry_w + ol * wet_w;
      out[1] = dr * dry_w + or * wet_w;
    }
  }

private:
  float env_l_, env_r_;
  float lp_l_, lp_r_;
  float thresh_, ratio_, atk_coef_, rel_coef_;
  float parallel_, drive_, mix_, tone_;
};
