#pragma once

namespace sh101 {
struct Program {
  const char *name;
  float cutoff, res, subLvl, pwm, glide;
  float fA, fD, fS, fR, aA, aD, aS, aR;
};
inline constexpr Program kPrograms[] = {
    {"101 Init", 680, 0.55f, 0.65f, 0.35f, 0.05f, 0.003f, 0.25f, 0.2f, 0.18f,
     0.002f, 0.2f, 0.85f, 0.12f},
    {"Acid", 420, 0.82f, 0.45f, 0.7f, 0.02f, 0.001f, 0.15f, 0.0f, 0.08f, 0.001f,
     0.35f, 0.0f, 0.1f},
    {"Sub Bass", 180, 0.48f, 0.92f, 0.15f, 0.08f, 0.005f, 0.4f, 0.35f, 0.25f,
     0.003f, 0.5f, 0.9f, 0.2f},
    {"Lead", 2200, 0.62f, 0.35f, 0.55f, 0.12f, 0.008f, 0.22f, 0.15f, 0.15f,
     0.002f, 0.18f, 0.65f, 0.12f},
};
inline constexpr int kNumPrograms = 4;
} // namespace sh101