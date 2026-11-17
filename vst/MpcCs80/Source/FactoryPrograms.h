#pragma once

#include <cstddef>

namespace cs80 {

struct ProgramSnapshot {
  const char *name;
  float cutoff, res, layerBalance, pwm, ringAmt, subLvl, aftertouchAmt;
  float fAttack, fDecay, fSustain, fRelease;
  float aAttack, aDecay, aSustain, aRelease;
};

inline constexpr ProgramSnapshot kFactoryPrograms[] = {
    {"Brass Init", 2200, 0.42f, 0.48f, 0.4f, 0.1f, 0.5f, 0.55f,
     0.02f, 0.45f, 0.35f, 0.4f, 0.008f, 0.28f, 0.78f, 0.35f},
    {"Blade Sweep", 180, 0.68f, 0.35f, 0.25f, 0.05f, 0.35f, 0.65f,
     0.15f, 1.2f, 0.5f, 0.8f, 0.01f, 0.35f, 0.7f, 0.5f},
    {"String Pad", 1400, 0.22f, 0.72f, 0.15f, 0.02f, 0.25f, 0.4f,
     0.06f, 1.5f, 0.6f, 0.9f, 0.02f, 0.55f, 0.85f, 0.55f},
    {"AT Lead", 3200, 0.48f, 0.5f, 0.55f, 0.12f, 0.2f, 0.85f,
     0.003f, 0.2f, 0.15f, 0.12f, 0.002f, 0.18f, 0.55f, 0.12f},
};

inline constexpr std::size_t kNumFactoryPrograms =
    sizeof(kFactoryPrograms) / sizeof(kFactoryPrograms[0]);

} // namespace cs80