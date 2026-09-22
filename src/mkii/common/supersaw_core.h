#pragma once
/*
 * Shared supersaw engine for mkII units (Virus, JP-8000, rage rap variants).
 * Header-only — include from osc.h; no malloc.
 */
#include <math.h>

struct SupersawTuning {
  float detune_cents_max;
  float hp_hz_default;
  float sub_mix_default;
  float voice_gain;
};

struct SupersawState {
  static constexpr int k_max_voices = 7;

  float phase[k_max_voices];
  float hp_z;
  float sub_phase;

  void resetPhases() {
    for (int i = 0; i < k_max_voices; ++i) {
      phase[i] = static_cast<float>(i) * 0.13f;
    }
    sub_phase = 0.f;
    hp_z = 0.f;
  }

  static float onePoleHp(float in, float &z, float hz) {
    const float g = 1.f - expf(-6.28318530718f * hz / 48000.f);
    z += g * (in - z);
    return in - z;
  }

  static float saw(float ph) {
    ph = ph - floorf(ph);
    return 2.f * ph - 1.f;
  }

  static float voiceDetune(int voice_index, int num_voices, float spread_norm, float cents_max) {
    if (num_voices <= 1) {
      return 1.f;
    }
    const float center = (num_voices - 1) * 0.5f;
    const float off = (static_cast<float>(voice_index) - center) / center;
    const float cents = off * spread_norm * cents_max;
    return powf(2.f, cents / 1200.f);
  }

  float processSample(float w0, int num_voices, float spread_norm, float sub_amt, float hp_hz,
                      float amp, const SupersawTuning &tune) {
    if (num_voices < 2) {
      num_voices = 2;
    }
    if (num_voices > k_max_voices) {
      num_voices = k_max_voices;
    }

    float sum = 0.f;
    for (int v = 0; v < num_voices; ++v) {
      const float wv = w0 * voiceDetune(v, num_voices, spread_norm, tune.detune_cents_max);
      phase[v] += wv;
      if (phase[v] >= 1.f) {
        phase[v] -= floorf(phase[v]);
      }
      sum += saw(phase[v]) * tune.voice_gain;
    }
    sum /= static_cast<float>(num_voices);

    sub_phase += w0;
    if (sub_phase >= 1.f) {
      sub_phase -= floorf(sub_phase);
    }
    const float sub = sinf(6.28318530718f * sub_phase) * sub_amt;

    float out = sum + sub;
    out = onePoleHp(out, hp_z, hp_hz);
    return out * amp;
  }
};

inline SupersawTuning supersaw_tuning_virus() {
  SupersawTuning t{};
  t.detune_cents_max = 38.f;
  t.hp_hz_default = 140.f;
  t.sub_mix_default = 0.35f;
  t.voice_gain = 1.f;
  return t;
}

inline SupersawTuning supersaw_tuning_jp8000() {
  SupersawTuning t{};
  t.detune_cents_max = 28.f;
  t.hp_hz_default = 90.f;
  t.sub_mix_default = 0.25f;
  t.voice_gain = 1.f;
  return t;
}

inline SupersawTuning supersaw_tuning_rage() {
  SupersawTuning t{};
  t.detune_cents_max = 48.f;
  t.hp_hz_default = 115.f;
  t.sub_mix_default = 0.58f;
  t.voice_gain = 1.05f;
  return t;
}
