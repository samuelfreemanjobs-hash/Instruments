#pragma once

namespace minimoog {
struct Program {
  const char *name;
  float cutoff, res, glide, drive, osc2Det, osc3Det, noiseLvl;
  float fA, fD, fS, fR, aA, aD, aS, aR;
};
inline constexpr Program kPrograms[] = {
    {"Model D Init", 520.0f, 0.45f, 0.08f, 1.6f, 0.07f, -0.07f, 0.04f, 0.01f,
     0.35f, 0.28f, 0.22f, 0.008f, 0.4f, 0.82f, 0.22f},
    {"Thick Bass", 220.0f, 0.52f, 0.04f, 2.2f, 0.0f, 0.0f, 0.02f, 0.005f,
     0.45f, 0.35f, 0.3f, 0.003f, 0.55f, 0.92f, 0.28f},
    {"Scream Lead", 4200.0f, 0.78f, 0.15f, 2.8f, 0.12f, 0.05f, 0.08f, 0.002f,
     0.12f, 0.08f, 0.0f, 0.002f, 0.15f, 0.5f, 0.12f},
    {"Funk Clav", 1800.0f, 0.38f, 0.02f, 1.4f, 0.03f, -0.03f, 0.06f, 0.003f,
     0.22f, 0.18f, 0.12f, 0.001f, 0.28f, 0.0f, 0.12f},
};
inline constexpr int kNumPrograms = 4;
} // namespace minimoog