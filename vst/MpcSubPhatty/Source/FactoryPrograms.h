#pragma once

#include <cstddef>

namespace subphatty {

struct ProgramSnapshot {
  const char *name;
  float cutoff, res, multidrive, envAmt, subLvl, waveShape, osc2Detune, glide, noiseLvl;
  bool slope12dB;
  float fAttack, fDecay, fSustain, fRelease;
  float aAttack, aDecay, aSustain, aRelease;
};

inline constexpr ProgramSnapshot kFactoryPrograms[] = {
    {"Init Sub Phatty", 420, 0.42f, 0.45f, 4500, 0.75f, 0.35f, 0.09f, 0.03f, 0.02f, false,
     0.005f, 0.28f, 0.15f, 0.14f, 0.004f, 0.35f, 0.85f, 0.14f},
    {"Sub Phatty 808", 95, 0.38f, 0.55f, 2200, 0.9f, 0.25f, 0.02f, 0.06f, 0.01f, false,
     0.008f, 0.5f, 0.2f, 0.35f, 0.003f, 0.55f, 0.92f, 0.4f},
    {"Growl Reese Bass", 280, 0.62f, 0.72f, 5200, 0.65f, 0.55f, 0.14f, 0.04f, 0.05f, false,
     0.004f, 0.22f, 0.1f, 0.12f, 0.003f, 0.28f, 0.7f, 0.18f},
    {"Acid Phatty", 420, 0.78f, 0.85f, 6800, 0.5f, 0.72f, 0.08f, 0.01f, 0.03f, true,
     0.002f, 0.15f, 0.0f, 0.08f, 0.002f, 0.18f, 0.0f, 0.06f},
    {"Grit Moog Bass", 180, 0.48f, 0.68f, 3800, 0.85f, 0.3f, 0.03f, 0.05f, 0.02f, false,
     0.01f, 0.42f, 0.25f, 0.3f, 0.004f, 0.45f, 0.88f, 0.32f},
    {"Phatty Slide", 120, 0.35f, 0.4f, 1800, 0.95f, 0.2f, 0.01f, 0.35f, 0.0f, false,
     0.012f, 0.55f, 0.35f, 0.45f, 0.005f, 0.6f, 0.9f, 0.45f},
    {"Scream Lead", 5200, 0.7f, 0.9f, 7200, 0.35f, 0.8f, 0.12f, 0.02f, 0.06f, false,
     0.003f, 0.18f, 0.05f, 0.1f, 0.002f, 0.2f, 0.45f, 0.12f},
    {"12dB Punch", 650, 0.55f, 0.5f, 4000, 0.7f, 0.4f, 0.06f, 0.02f, 0.04f, true,
     0.006f, 0.3f, 0.12f, 0.15f, 0.003f, 0.32f, 0.75f, 0.2f},
    {"24dB Sub", 75, 0.45f, 0.35f, 1500, 1.0f, 0.15f, 0.0f, 0.08f, 0.0f, false,
     0.01f, 0.6f, 0.3f, 0.4f, 0.004f, 0.5f, 0.95f, 0.38f},
    {"Detroit Growl", 240, 0.58f, 0.75f, 4800, 0.72f, 0.48f, 0.11f, 0.03f, 0.03f, false,
     0.004f, 0.25f, 0.08f, 0.14f, 0.003f, 0.3f, 0.65f, 0.16f},
    {"Noise Bass", 350, 0.52f, 0.6f, 3500, 0.55f, 0.38f, 0.04f, 0.01f, 0.18f, false,
     0.005f, 0.35f, 0.18f, 0.2f, 0.003f, 0.4f, 0.8f, 0.22f},
    {"Battle Phat", 200, 0.5f, 0.65f, 3000, 0.88f, 0.32f, 0.05f, 0.04f, 0.02f, false,
     0.007f, 0.38f, 0.22f, 0.25f, 0.004f, 0.42f, 0.82f, 0.28f},
    {"West Coast Sub", 140, 0.4f, 0.48f, 2600, 0.92f, 0.28f, 0.07f, 0.12f, 0.01f, false,
     0.009f, 0.48f, 0.28f, 0.35f, 0.004f, 0.48f, 0.88f, 0.35f},
    {"Narrow Pulse", 900, 0.65f, 0.82f, 5500, 0.45f, 0.92f, 0.1f, 0.0f, 0.02f, true,
     0.002f, 0.2f, 0.0f, 0.08f, 0.002f, 0.15f, 0.0f, 0.06f},
    {"Filter Whomp", 160, 0.88f, 0.7f, 9000, 0.6f, 0.5f, 0.06f, 0.02f, 0.03f, false,
     0.001f, 0.12f, 0.0f, 0.05f, 0.001f, 0.1f, 0.0f, 0.04f},
    {"Melodee Init", 380, 0.42f, 0.45f, 4500, 0.75f, 0.35f, 0.09f, 0.03f, 0.02f, false,
     0.005f, 0.28f, 0.15f, 0.14f, 0.004f, 0.35f, 0.85f, 0.14f},
};

inline constexpr std::size_t kNumFactoryPrograms =
    sizeof(kFactoryPrograms) / sizeof(kFactoryPrograms[0]);

} // namespace subphatty