#pragma once
/* PORT_COMPLETE — Cardo Got Wings–style drum spring: short bright tank + glow. */
#include "processor.h"
#include "unit_revfx.h"

#include <math.h>

class CardoSpringRevfx : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0x10000U; }

  enum {
    SPRG = 0U,
    TONE = 1U,
    SIZE = 2U,
    DAMP = 3U,
    PRE = 4U,
    GLOW = 5U,
    MIX = 6U,
    DRUM = 7U,
  };

  enum {
    DRUM_GOT_WINGS = 0U,
    DRUM_OPEN = 1U,
    DRUM_TIGHT = 2U,
  };

  void setParameter(uint8_t index, int32_t value) override final {
    const float n = param_10bit_to_f32(value);
    switch (index) {
    case SPRG:
      spring_ = n;
      break;
    case TONE:
      tone_ = n;
      break;
    case SIZE:
      size_ = 0.02f + n * n * 0.35f;
      break;
    case DAMP:
      damp_ = 0.2f + n * 0.75f;
      break;
    case PRE:
      pre_ms_ = n * 25.f;
      break;
    case GLOW:
      glow_ = n;
      break;
    case MIX:
      mix_ = (value / 1000.f + 1.f) * 0.5f;
      break;
    case DRUM:
      drum_preset_ = static_cast<uint8_t>(value);
      applyDrumPreset();
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t index, int32_t value) const override final {
    static const char *k_drum[] = {"WINGZ", "OPEN ", "TIGHT"};
    if (index == DRUM && value >= 0 && value < 3) {
      return k_drum[value];
    }
    return nullptr;
  }

  void init(float *allocated_buffer) override final {
    buffer_ = allocated_buffer;
    reset();
  }

  void teardown() override final { buffer_ = nullptr; }

  void reset() override final {
    if (buffer_) {
      for (uint32_t i = 0; i < getBufferSize(); ++i) {
        buffer_[i] = 0.f;
      }
    }
    comb_ph_[0] = comb_ph_[1] = comb_ph_[2] = 0U;
    spring_ = param_10bit_to_f32(520);
    tone_ = param_10bit_to_f32(580);
    size_ = 0.12f;
    damp_ = 0.45f;
    pre_ms_ = 3.f;
    glow_ = param_10bit_to_f32(450);
    mix_ = 0.42f;
    drum_preset_ = DRUM_GOT_WINGS;
    applyDrumPreset();
  }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    static constexpr float k_fs = 48000.f;
    if (!buffer_) {
      for (uint32_t i = 0; i < frames * 2; ++i) {
        out[i] = in[i];
      }
      return;
    }

    const uint32_t len = getBufferSize();
    const float dry_w = 1.f - mix_;
    const float wet_w = mix_;
    const float delay_s = size_ * (0.7f + spring_ * 0.6f);
    const uint32_t d0 = static_cast<uint32_t>(delay_s * k_fs * 0.021f) & (len - 1U);
    const uint32_t d1 = static_cast<uint32_t>(delay_s * k_fs * 0.031f) & (len - 1U);
    const uint32_t d2 = static_cast<uint32_t>(delay_s * k_fs * 0.047f) & (len - 1U);
    const float fb = 0.35f + spring_ * 0.45f;
    const float damp_c = damp_;
    const float tone_g = 1.f - expf(-6.28318530718f * (900.f + tone_ * 5000.f) / k_fs);
    float lp_l = 0.f;
    float lp_r = 0.f;

    for (const float *out_end = out + frames * 2; out != out_end; in += 2, out += 2) {
      const float mono = 0.5f * (in[0] + in[1]);
      const float pre = mono;

      const float t0 = buffer_[(comb_ph_[0] + d0) & (len - 1U)];
      const float t1 = buffer_[(comb_ph_[1] + d1) & (len - 1U)];
      const float t2 = buffer_[(comb_ph_[2] + d2) & (len - 1U)];
      float wet = (t0 + t1 * 0.85f + t2 * 0.7f) * 0.45f;

      wet += glow_ * 0.15f * sinf(6.28318530718f * wet * (2.f + glow_));

      lp_l += tone_g * (wet - lp_l);
      lp_r += tone_g * (wet - lp_r);
      wet = 0.5f * (lp_l + lp_r);

      buffer_[comb_ph_[0]] = pre + wet * fb * (1.f - damp_c * 0.5f);
      comb_ph_[0] = (comb_ph_[0] + 1U) & (len - 1U);
      comb_ph_[1] = (comb_ph_[1] + 3U) & (len - 1U);
      comb_ph_[2] = (comb_ph_[2] + 5U) & (len - 1U);

      out[0] = in[0] * dry_w + (in[0] * 0.35f + wet * 0.65f) * wet_w;
      out[1] = in[1] * dry_w + (in[1] * 0.35f + wet * 0.65f) * wet_w;
    }
  }

private:
  void applyDrumPreset() {
    switch (drum_preset_) {
    case DRUM_GOT_WINGS:
      spring_ = 0.55f;
      tone_ = 0.62f;
      size_ = 0.14f;
      damp_ = 0.42f;
      glow_ = 0.48f;
      break;
    case DRUM_OPEN:
      spring_ = 0.72f;
      size_ = 0.22f;
      damp_ = 0.28f;
      break;
    case DRUM_TIGHT:
      spring_ = 0.38f;
      size_ = 0.08f;
      damp_ = 0.62f;
      glow_ = 0.25f;
      break;
    default:
      break;
    }
  }

  float *buffer_;
  uint32_t comb_ph_[3];
  float spring_, tone_, size_, damp_, pre_ms_, glow_, mix_;
  uint8_t drum_preset_;
};
