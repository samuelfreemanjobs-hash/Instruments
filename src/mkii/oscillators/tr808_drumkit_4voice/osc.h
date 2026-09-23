#pragma once
/* PORT_COMPLETE — TR-808 kit: kick/snare/CH/OH via MIDI note map (owner locked). */
#include "processor.h"
#include "unit_osc.h"

#include <math.h>

class Osc : public Processor {
public:
  static constexpr uint8_t k_note_kick = 36U;
  static constexpr uint8_t k_note_snare = 38U;
  static constexpr uint8_t k_note_ch = 42U;
  static constexpr uint8_t k_note_oh = 46U;

  uint32_t getBufferSize() const override final { return 0; }

  enum {
    TONE = 0U,
    PUNCH = 1U,
    DECAY = 2U,
    SNARE = 3U,
    HATLEN = 4U,
    OHDEC = 5U,
    DRIVE = 6U,
    LEVEL = 7U,
    TUNE = 8U,
  };

  void setParameter(uint8_t index, int32_t value) override final {
    const float n = param_10bit_to_f32(value);
    switch (index) {
    case TONE:
      tone_ = n;
      break;
    case PUNCH:
      punch_ = n;
      break;
    case DECAY:
      kick_decay_ = 0.08f + n * n * 2.2f;
      break;
    case SNARE:
      snare_snap_ = n;
      break;
    case HATLEN:
      ch_decay_ = 0.01f + n * 0.08f;
      break;
    case OHDEC:
      oh_decay_ = 0.04f + n * n * 0.45f;
      break;
    case DRIVE:
      drive_ = n;
      break;
    case LEVEL:
      level_ = 0.15f + n * 0.85f;
      break;
    case TUNE:
      tune_mul_ = 0.5f + n * 1.5f;
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t, int32_t) const override final { return nullptr; }
  void init(float *) override final { reset(); }

  void reset() override final {
    rng_ = 0x12345678U;
    tone_ = param_10bit_to_f32(512);
    punch_ = param_10bit_to_f32(614);
    kick_decay_ = 0.45f;
    snare_snap_ = param_10bit_to_f32(655);
    ch_decay_ = 0.035f;
    oh_decay_ = 0.18f;
    drive_ = param_10bit_to_f32(409);
    level_ = 0.9f;
    tune_mul_ = 1.f;
    kick_ = snare_ = ch_ = oh_ = {};
  }

  void setPitch(float w0) { (void)w0; }
  void setShapeLfo(float lfo) { (void)lfo; }

  void noteOn(uint8_t note, uint8_t velo) override final {
    const float vel = static_cast<float>(velo) / 127.f;
    switch (note) {
    case k_note_kick:
      triggerKick(vel);
      break;
    case k_note_snare:
      triggerSnare(vel);
      break;
    case k_note_ch:
      triggerHat(ch_, vel, ch_decay_);
      break;
    case k_note_oh:
      triggerHat(oh_, vel, oh_decay_);
      break;
    default:
      break;
    }
  }

  void noteOff(uint8_t note) override final { (void)note; }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    static constexpr float k_fs = 48000.f;
    (void)in;
    const float kick_decay_coef = expf(-6.f / (kick_decay_ * k_fs));

    for (const float *out_end = out + frames; out != out_end; ++out) {
      float sig = 0.f;
      sig += processKick(kick_decay_coef, k_fs);
      sig += processSnare(k_fs);
      sig += processHat(ch_, k_fs);
      sig += processHat(oh_, k_fs);

      const float g = 1.f + drive_ * 3.5f;
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
  struct HatVoice {
    float env;
    float lp;
  };

  struct SnareVoice {
    float env;
    float tone_phase;
    float tone_w;
    float noise_lp;
  };

  struct KickVoice {
    float env;
    float phase;
    float w0;
    float pitch_mul;
    float lp;
  };

  uint32_t rngStep() {
    rng_ ^= rng_ << 13;
    rng_ ^= rng_ >> 17;
    rng_ ^= rng_ << 5;
    return rng_;
  }

  float noise() { return (static_cast<float>(rngStep() & 0xffffU) / 32768.f - 1.f); }

  void triggerKick(float vel) {
    kick_.env = vel * (0.7f + punch_ * 0.3f);
    kick_.phase = 0.f;
    kick_.pitch_mul = 1.8f + punch_ * 2.5f;
    kick_.w0 = 0.0035f * tune_mul_;
    kick_.lp = 0.f;
  }

  void triggerSnare(float vel) {
    snare_.env = vel * (0.65f + snare_snap_ * 0.35f);
    snare_.tone_phase = 0.f;
    snare_.tone_w = 0.012f * tune_mul_;
    snare_.noise_lp = 0.f;
  }

  void triggerHat(HatVoice &h, float vel, float decay_hint) {
    (void)decay_hint;
    h.env = vel * (0.55f + tone_ * 0.35f);
    h.lp = 0.f;
  }

  float processKick(float decay_coef, float fs) {
    if (kick_.env < 0.0001f) {
      return 0.f;
    }
    kick_.pitch_mul += (1.f - kick_.pitch_mul) * (1.f / (kick_decay_ * fs * 0.15f));
    kick_.phase += kick_.w0 * kick_.pitch_mul;
    if (kick_.phase >= 1.f) {
      kick_.phase -= floorf(kick_.phase);
    }
    float s = sinf(6.28318530718f * kick_.phase);
    s += (2.f * kick_.phase - 1.f) * tone_ * 0.15f;
    kick_.env *= decay_coef;
    const float g = 1.f - expf(-6.28318530718f * (40.f + tone_ * 80.f) / fs);
    kick_.lp += g * (s - kick_.lp);
    return kick_.lp * kick_.env * (0.9f + punch_ * 0.2f);
  }

  float processSnare(float fs) {
    if (snare_.env < 0.0001f) {
      return 0.f;
    }
    snare_.tone_phase += snare_.tone_w;
    if (snare_.tone_phase >= 1.f) {
      snare_.tone_phase -= 1.f;
    }
    const float body = sinf(6.28318530718f * snare_.tone_phase) * (0.35f + snare_snap_ * 0.25f);
    float n = noise();
    const float g = 1.f - expf(-6.28318530718f * (800.f + tone_ * 4000.f) / fs);
    snare_.noise_lp += g * (n - snare_.noise_lp);
    snare_.env *= expf(-6.f / ((0.06f + snare_snap_ * 0.12f) * fs));
    return (body + snare_.noise_lp * (0.5f + snare_snap_ * 0.5f)) * snare_.env;
  }

  float processHat(HatVoice &h, float fs) {
    if (h.env < 0.0001f) {
      return 0.f;
    }
    float n = noise();
    const float g = 1.f - expf(-6.28318530718f * (5000.f + tone_ * 8000.f) / fs);
    h.lp += g * (n - h.lp);
    const float d = (&h == &ch_) ? ch_decay_ : oh_decay_;
    h.env *= expf(-6.f / (d * fs));
    return h.lp * h.env * 0.55f;
  }

  KickVoice kick_;
  SnareVoice snare_;
  HatVoice ch_;
  HatVoice oh_;
  uint32_t rng_;

  float tone_, punch_, kick_decay_, snare_snap_, ch_decay_, oh_decay_, drive_, level_, tune_mul_;
};
