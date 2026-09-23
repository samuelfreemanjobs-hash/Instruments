#pragma once
/* PORT_COMPLETE — trap-tilted broken cassette: digital glitch table + crush + hiss. */
#include "processor.h"
#include "unit_modfx.h"

#include <math.h>

class TrapCassetteFx : public Processor {
public:
  uint32_t getBufferSize() const override final { return 0x2000U; }

  enum {
    GLIT = 0U,
    STEP = 1U,
    CRSH = 2U,
    HISS = 3U,
    TRAP = 4U,
    WOW = 5U,
    MIX = 6U,
    LVL = 7U,
  };

  static constexpr float k_glitch_table[16] = {
      1.f, 0.f, 0.35f, -0.2f, 0.8f, 0.f, 0.5f, -0.45f,
      0.25f, 1.f, 0.f, 0.6f, -0.15f, 0.4f, 0.f, 0.9f,
  };

  void setParameter(uint8_t index, int32_t value) override final {
    switch (index) {
    case GLIT:
      glitch_amt_ = param_10bit_to_f32(value);
      break;
    case STEP:
      step_idx_ = static_cast<uint8_t>(value >> 6);
      break;
    case CRSH:
      crush_bits_ = 4.f + param_10bit_to_f32(value) * 10.f;
      break;
    case HISS:
      hiss_ = param_10bit_to_f32(value);
      break;
    case TRAP:
      trap_tight_ = param_10bit_to_f32(value);
      break;
    case WOW:
      wow_ = param_10bit_to_f32(value);
      break;
    case MIX:
      mix_ = (value / 1000.f + 1.f) * 0.5f;
      break;
    case LVL:
      level_ = 0.5f + param_10bit_to_f32(value) * 0.5f;
      break;
    default:
      break;
    }
  }

  const char *getParameterStrValue(uint8_t, int32_t) const override final { return nullptr; }

  void init(float *allocated_buffer) override final {
    buffer_ = allocated_buffer;
    reset();
  }

  void teardown() override final { buffer_ = nullptr; }

  void reset() override final {
    rng_ = 0x9e3779b9U;
    hold_l_ = hold_r_ = 0.f;
    wow_ph_ = 0.f;
    write_pos_ = 0U;
    beat_gate_ = 0.f;
    glitch_amt_ = param_10bit_to_f32(614);
    step_idx_ = 8U;
    crush_bits_ = 8.f;
    hiss_ = param_10bit_to_f32(368);
    trap_tight_ = param_10bit_to_f32(655);
    wow_ = param_10bit_to_f32(286);
    mix_ = 0.35f;
    level_ = 0.85f;
  }

  void setTempo(float bpm) override final { (void)bpm; }

  void tempo4ppqnTick(uint32_t) override final {
    beat_gate_ = 1.f;
  }

  void process(const float *__restrict in, float *__restrict out, uint32_t frames) override final {
    static constexpr float k_fs = 48000.f;
    const float mix_w = mix_;
    const float dry_w = 1.f - mix_w;
    const float trap_bias = trap_tight_ * 0.85f;

    for (const float *out_end = out + frames * 2; out != out_end; in += 2, out += 2) {
      beat_gate_ *= 0.9995f;

      float l = in[0];
      float r = in[1];

      wow_ph_ += (0.4f + wow_ * 2.5f) / k_fs;
      if (wow_ph_ >= 1.f) {
        wow_ph_ -= 1.f;
      }
      const float wow_mod = 1.f + sinf(6.28318530718f * wow_ph_) * wow_ * 0.012f;

      const float env = fabsf(l) + fabsf(r);
      uint32_t rstep = rngStep();
      const float gate = glitch_amt_ * (0.35f + beat_gate_ * trap_bias + env * (1.f - trap_bias));
      if ((rstep & 0xffffU) < static_cast<uint32_t>(gate * 65535.f)) {
        const uint8_t ti = (step_idx_ + (rstep & 0xfU)) & 0xfU;
        const float g = k_glitch_table[ti];
        hold_l_ = l * g;
        hold_r_ = r * g;
      } else {
        hold_l_ = hold_l_ * 0.92f + l * 0.08f;
        hold_r_ = hold_r_ * 0.92f + r * 0.08f;
      }

      l = hold_l_ * wow_mod;
      r = hold_r_ * wow_mod;

      const float crush = powf(2.f, crush_bits_);
      l = floorf(l * crush) / crush;
      r = floorf(r * crush) / crush;

      if (buffer_) {
        buffer_[write_pos_] = 0.5f * (l + r);
        write_pos_ = (write_pos_ + 1U) & (getBufferSize() - 1U);
      }

      const float n = (static_cast<float>(rngStep() & 0xffffU) / 32768.f - 1.f) * hiss_ * 0.08f;
      l += n;
      r += n * 0.97f;

      out[0] = (in[0] * dry_w + l * mix_w) * level_;
      out[1] = (in[1] * dry_w + r * mix_w) * level_;
    }
  }

private:
  uint32_t rngStep() {
    rng_ ^= rng_ << 13;
    rng_ ^= rng_ >> 17;
    rng_ ^= rng_ << 5;
    return rng_;
  }

  float *buffer_;
  uint32_t rng_;
  uint32_t write_pos_;
  float hold_l_, hold_r_;
  float wow_ph_;
  float beat_gate_;
  float glitch_amt_, crush_bits_, hiss_, trap_tight_, wow_, mix_, level_;
  uint8_t step_idx_;
};
