#pragma once

namespace juno106 {
struct Program {
  const char *name;
  float cutoff, res, chorus;
  float saw, pulse, pw, sub;
  float attack, decay, sustain, release;
  float lfoRate, lfoPwm;
};

inline constexpr int kNumPrograms = 4;
inline constexpr Program kPrograms[] = {
    {"Init", 2000.0f, 0.15f, 0.35f, 0.8f, 0.0f, 0.5f, 0.3f, 0.005f, 0.4f,
     0.85f, 0.3f, 3.0f, 0.0f},
    {"Chorus Pad", 3200.0f, 0.2f, 0.65f, 0.75f, 0.2f, 0.45f, 0.35f, 0.35f,
     0.6f, 0.88f, 1.2f, 2.5f, 0.15f},
    {"Funk Bass", 420.0f, 0.35f, 0.1f, 0.9f, 0.6f, 0.25f, 0.7f, 0.002f,
     0.18f, 0.75f, 0.15f, 0.4f, 0.35f},
    {"Brass Stab", 1800.0f, 0.45f, 0.25f, 0.85f, 0.15f, 0.55f, 0.15f, 0.003f,
     0.22f, 0.65f, 0.25f, 5.0f, 0.05f},
};
} // namespace juno106