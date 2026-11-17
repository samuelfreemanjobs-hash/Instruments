#pragma once

#include <cstddef>

namespace voyager {

struct ProgramSnapshot {
  const char *name;
  float cutoff;
  float res;
  float envAmt;
  float drive;
  float glide;
  float waveShape;
  float osc2Detune;
  float osc3Sub;
  float noiseLvl;
  float fAttack, fDecay, fSustain, fRelease;
  float aAttack, aDecay, aSustain, aRelease;
};

// 16 on-board programs; expand via params.json / future bank load (target 200-style library).
inline constexpr ProgramSnapshot kFactoryPrograms[] = {
    {"Init Voyager", 650, 0.35f, 3200, 1.8f, 0.04f, 0.55f, 0.07f, 0.65f, 0.03f,
     0.015f, 0.35f, 0.25f, 0.18f, 0.008f, 0.4f, 0.8f, 0.18f},
    {"G-Funk Worm Lead", 4200, 0.62f, 5800, 2.2f, 0.12f, 0.72f, 0.11f, 0.2f, 0.02f,
     0.01f, 0.28f, 0.1f, 0.22f, 0.006f, 0.25f, 0.55f, 0.35f},
    {"Sub 808 Moog Bass", 220, 0.48f, 1800, 2.6f, 0.08f, 0.35f, 0.03f, 0.9f, 0.01f,
     0.02f, 0.55f, 0.35f, 0.4f, 0.005f, 0.45f, 0.9f, 0.35f},
    {"Battlecat Low", 380, 0.55f, 2600, 2.4f, 0.02f, 0.48f, 0.05f, 0.75f, 0.04f,
     0.008f, 0.4f, 0.2f, 0.15f, 0.004f, 0.35f, 0.85f, 0.2f},
    {"Warren G Moog", 520, 0.42f, 3000, 2.1f, 0.06f, 0.5f, 0.08f, 0.8f, 0.02f,
     0.012f, 0.38f, 0.3f, 0.25f, 0.01f, 0.42f, 0.75f, 0.28f},
    {"Roger Talk Lead", 6800, 0.7f, 7200, 2.0f, 0.05f, 0.85f, 0.14f, 0.15f, 0.06f,
     0.005f, 0.2f, 0.05f, 0.18f, 0.003f, 0.18f, 0.4f, 0.12f},
    {"P-Funk Rubber", 1100, 0.38f, 4500, 2.3f, 0.1f, 0.6f, 0.12f, 0.55f, 0.05f,
     0.02f, 0.5f, 0.4f, 0.35f, 0.015f, 0.5f, 0.7f, 0.45f},
    {"West Coast Stab", 2400, 0.58f, 6500, 2.5f, 0.0f, 0.78f, 0.06f, 0.35f, 0.08f,
     0.003f, 0.22f, 0.0f, 0.1f, 0.002f, 0.2f, 0.0f, 0.08f},
    {"Voyager Brass", 3400, 0.52f, 5000, 2.15f, 0.03f, 0.65f, 0.09f, 0.4f, 0.07f,
     0.01f, 0.32f, 0.25f, 0.2f, 0.008f, 0.35f, 0.65f, 0.22f},
    {"1980s Moog Bass", 310, 0.5f, 2200, 2.55f, 0.04f, 0.42f, 0.04f, 0.85f, 0.02f,
     0.015f, 0.48f, 0.28f, 0.3f, 0.005f, 0.5f, 0.88f, 0.32f},
    {"Slide Sub G", 180, 0.4f, 1500, 2.7f, 0.35f, 0.3f, 0.02f, 0.95f, 0.0f,
     0.025f, 0.6f, 0.4f, 0.5f, 0.006f, 0.55f, 0.92f, 0.4f},
    {"Detroit Moog", 450, 0.46f, 2800, 2.35f, 0.07f, 0.52f, 0.1f, 0.7f, 0.03f,
     0.01f, 0.36f, 0.22f, 0.22f, 0.007f, 0.38f, 0.78f, 0.25f},
    {"Night Funk Lead", 5200, 0.65f, 6000, 2.05f, 0.09f, 0.8f, 0.13f, 0.25f, 0.04f,
     0.008f, 0.24f, 0.12f, 0.2f, 0.004f, 0.22f, 0.5f, 0.3f},
    {"Dry Square Bass", 280, 0.32f, 1900, 2.45f, 0.01f, 0.95f, 0.02f, 0.6f, 0.0f,
     0.012f, 0.42f, 0.3f, 0.25f, 0.004f, 0.4f, 0.9f, 0.22f},
    {"Wide Detune Pad", 1800, 0.28f, 3800, 1.6f, 0.15f, 0.45f, 0.18f, 0.5f, 0.02f,
     0.04f, 0.7f, 0.55f, 0.6f, 0.02f, 0.65f, 0.8f, 0.55f},
    {"Noise Stab FX", 900, 0.72f, 4000, 2.8f, 0.0f, 0.7f, 0.15f, 0.2f, 0.22f,
     0.002f, 0.15f, 0.0f, 0.08f, 0.001f, 0.12f, 0.0f, 0.06f},
};

inline constexpr std::size_t kNumFactoryPrograms =
    sizeof(kFactoryPrograms) / sizeof(kFactoryPrograms[0]);

} // namespace voyager