#pragma once

#include <cstddef>

namespace messenger {

struct ProgramSnapshot {
  const char *name;
  float cutoff, res, resBass, fold, fmAmt, envAmt, subLvl, noiseLvl, waveShape, osc2Detune, glide;
  float fAttack, fDecay, fSustain, fRelease;
  float aAttack, aDecay, aSustain, aRelease;
};

inline constexpr ProgramSnapshot kFactoryPrograms[] = {
    {"Messenger Init", 520, 0.38f, 0.55f, 0.2f, 0.1f, 3800, 0.7f, 0.03f, 0.45f, 0.06f, 0.04f,
     0.008f, 0.32f, 0.2f, 0.2f, 0.004f, 0.35f, 0.82f, 0.22f},
    {"Res Bass Trap", 180, 0.62f, 0.85f, 0.15f, 0.05f, 4200, 0.88f, 0.02f, 0.35f, 0.03f, 0.08f,
     0.006f, 0.4f, 0.28f, 0.35f, 0.003f, 0.5f, 0.9f, 0.4f},
    {"Fold Lead", 4200, 0.55f, 0.4f, 0.75f, 0.35f, 5500, 0.25f, 0.04f, 0.65f, 0.12f, 0.02f,
     0.004f, 0.22f, 0.08f, 0.15f, 0.002f, 0.2f, 0.5f, 0.12f},
    {"FM Metal", 2800, 0.48f, 0.5f, 0.4f, 0.72f, 4800, 0.35f, 0.06f, 0.7f, 0.15f, 0.0f,
     0.003f, 0.18f, 0.05f, 0.1f, 0.002f, 0.15f, 0.45f, 0.1f},
    {"Phonk Sub", 95, 0.45f, 0.9f, 0.25f, 0.08f, 2200, 0.95f, 0.08f, 0.3f, 0.02f, 0.1f,
     0.01f, 0.55f, 0.35f, 0.45f, 0.004f, 0.55f, 0.92f, 0.38f},
    {"Analog Grit", 650, 0.7f, 0.65f, 0.55f, 0.15f, 5000, 0.6f, 0.12f, 0.5f, 0.08f, 0.03f,
     0.005f, 0.28f, 0.15f, 0.18f, 0.003f, 0.32f, 0.75f, 0.2f},
    {"Warm Pad Mono", 1400, 0.25f, 0.7f, 0.1f, 0.05f, 2800, 0.75f, 0.01f, 0.4f, 0.05f, 0.15f,
     0.02f, 0.6f, 0.5f, 0.55f, 0.015f, 0.65f, 0.8f, 0.5f},
    {"Crank Res", 320, 0.92f, 0.95f, 0.3f, 0.12f, 7200, 0.55f, 0.05f, 0.48f, 0.07f, 0.02f,
     0.002f, 0.15f, 0.0f, 0.08f, 0.002f, 0.12f, 0.0f, 0.06f},
};

inline constexpr std::size_t kNumFactoryPrograms =
    sizeof(kFactoryPrograms) / sizeof(kFactoryPrograms[0]);

} // namespace messenger