#pragma once
/* PORT_COMPLETE — 808-style FM cowbell + metal + clave (MIDI map). */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  static constexpr uint8_t k_note_cowbell = 37U;
  static constexpr uint8_t k_note_metal = 39U;
  static constexpr uint8_t k_note_clave = 41U;

  uint32_t getBufferSize() const override final { return 0; }

  enum {
    INDEX = 0U,
    RATIO = 1U,
    DECAY = 2U,
    TONE = 3U,
    METAL = 4U,
    LEVEL = 5U,
    TUNE = 6U,
    DRIVE = 7U,
  };

  void setParameter(uint8_t index, int32_t value) override final {
    const float n = param_10bit_to_f32(value);
    switch (index) {
    case INDEX:
      fm_index_ = n * 4.5f;
      break;
    case RATIO:
      fm_ratio_ = 1.4f + n * 1.2f;
      break;
    case DECAY:
      decay_sec_ = 0.04f + n * n * 0.55f;
      break;
    case TONE:
      tone_ = n;
      break;
    case METAL:
      metal_mix_ = n;
      break;
    case LEVEL:
      level_ = 0.2f + n * 0.8f;
      break;
    case TUNE:
      tune_mul_ = 0.6f + n * 1.6f;
      break;
    case DRIVE:
      drive_ = n;
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t, int32_t) const override final { return nullptr; }
  void init(float *) override final { reset(); }

  void reset() override final {
    cow_ = metal_ = clave_ = {};
    fm_index_ = param_10bit_to_f32(655) * 4.5f;
    fm_ratio_ = 1.75f;
    decay_sec_ = 0.22f;
    tone_ = param_10bit_to_f32(512);
    metal_mix_ = param_10bit_to_f32(450);
    level_ = 0.85f;
    tune_mul_ = 1.f;
    drive_ = param_10bit_to_f32(368);
  }

  void setPitch(float w0) { (void)w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void noteOn(uint8_t note, uint8_t velo) override final {
    const float vel = static_cast<float>(velo) / 127.f;
    switch (note) {
    case k_note_cowbell:
      triggerCowbell(vel);
      break;
    case k_note_metal:
      triggerMetal(vel);
      break;
    case k_note_clave:
      triggerClave(vel);
      break;
    default:
      break;
    }
  }

  void noteOff(uint8_t note) override final { (void)note; }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    static constexpr float k_fs = 48000.f;
    (void)in;
    const float decay_coef = expf(-6.f / (decay_sec_ * k_fs));

    for (const float *out_end = out + frames; out != out_end; ++out) {
      float sig = processCowbell(decay_coef, k_fs);
      sig += processMetal(decay_coef, k_fs);
      sig += processClave(decay_coef, k_fs);

      const float g = 1.f + drive_ * 2.8f;
      sig = tanhf(sig * g * level_);
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
  struct FmVoice {
    float env;
    float car_ph;
    float mod_ph;
    float car_w;
    float mod_w;
  };

  struct MetalVoice {
    float env;
    float ph_a;
    float ph_b;
    float wa;
    float wb;
  };

  struct ClaveVoice {
    float env;
    float ph;
    float w;
  };

  void triggerCowbell(float vel) {
    cow_.env = vel * (0.75f + tone_ * 0.25f);
    cow_.car_ph = cow_.mod_ph = 0.f;
    cow_.car_w = 0.014f * tune_mul_;
    cow_.mod_w = cow_.car_w * fm_ratio_;
  }

  void triggerMetal(float vel) {
    metal_.env = vel * (0.6f + metal_mix_ * 0.4f);
    metal_.ph_a = metal_.ph_b = 0.f;
    metal_.wa = 0.021f * tune_mul_;
    metal_.wb = metal_.wa * (2.01f + metal_mix_ * 0.8f);
  }

  void triggerClave(float vel) {
    clave_.env = vel * 0.9f;
    clave_.ph = 0.f;
    clave_.w = 0.035f * tune_mul_;
  }

  float processCowbell(float decay_coef, float fs) {
    if (cow_.env < 0.0001f) {
      return 0.f;
    }
    cow_.mod_ph += cow_.mod_w;
    if (cow_.mod_ph >= 1.f) {
      cow_.mod_ph -= 1.f;
    }
    cow_.car_ph += cow_.car_w;
    if (cow_.car_ph >= 1.f) {
      cow_.car_ph -= 1.f;
    }
    const float mod = sinf(6.28318530718f * cow_.mod_ph);
    float s = sinf(6.28318530718f * cow_.car_ph + mod * fm_index_);
    s += sinf(6.28318530718f * cow_.car_ph * 2.f) * tone_ * 0.12f;
    cow_.env *= decay_coef;
    return s * cow_.env * 0.7f;
  }

  float processMetal(float decay_coef, float fs) {
    (void)fs;
    if (metal_.env < 0.0001f) {
      return 0.f;
    }
    metal_.ph_a += metal_.wa;
    metal_.ph_b += metal_.wb;
    if (metal_.ph_a >= 1.f) {
      metal_.ph_a -= 1.f;
    }
    if (metal_.ph_b >= 1.f) {
      metal_.ph_b -= 1.f;
    }
    const float a = sinf(6.28318530718f * metal_.ph_a);
    const float b = sinf(6.28318530718f * metal_.ph_b);
    metal_.env *= decay_coef * (0.992f + metal_mix_ * 0.006f);
    return a * b * metal_.env * (0.35f + metal_mix_ * 0.45f);
  }

  float processClave(float decay_coef, float fs) {
    (void)fs;
    if (clave_.env < 0.0001f) {
      return 0.f;
    }
    clave_.ph += clave_.w;
    if (clave_.ph >= 1.f) {
      clave_.ph -= 1.f;
    }
    const float s = sinf(6.28318530718f * clave_.ph);
    clave_.env *= decay_coef * 0.965f;
    return s * clave_.env * 0.55f;
  }

  FmVoice cow_;
  MetalVoice metal_;
  ClaveVoice clave_;

  float fm_index_, fm_ratio_, decay_sec_, tone_, metal_mix_, level_, tune_mul_, drive_;
};
