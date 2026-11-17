#pragma once

#include <cstddef>

namespace dxfm {

struct ProgramSnapshot {
  const char *name;
  float brightness, feedback, algo;
  float op1_level, op2_level, op3_level, op4_level;
  float op1_ratio, op2_ratio, op3_ratio, op4_ratio;
  float attack, decay, sustain, release;
};

inline constexpr ProgramSnapshot kFactoryPrograms[] = {
    {"DxFm Init", 0.55f, 0.12f, 3, 1.0f, 0.7f, 0.5f, 0.6f, 1.0f, 2.0f, 1.0f,
     3.0f, 0.01f, 0.4f, 0.75f, 0.3f},
    {"EPiano", 0.72f, 0.18f, 6, 1.0f, 0.85f, 0.55f, 0.4f, 1.0f, 2.0f, 1.0f,
     2.0f, 0.002f, 1.2f, 0.0f, 0.45f},
    {"Bell", 0.88f, 0.05f, 5, 0.9f, 0.75f, 0.65f, 0.5f, 1.0f, 3.5f, 2.2f,
     4.0f, 0.001f, 0.8f, 0.0f, 0.9f},
    {"Slap Bass", 0.42f, 0.35f, 1, 1.0f, 0.55f, 0.35f, 0.25f, 0.5f, 1.0f, 2.0f,
     1.0f, 0.001f, 0.25f, 0.0f, 0.2f},
    {"G Lead", 0.78f, 0.22f, 7, 1.0f, 0.8f, 0.45f, 0.35f, 1.0f, 2.0f, 1.0f,
     3.0f, 0.004f, 0.35f, 0.6f, 0.15f},
    {"FM Pad", 0.48f, 0.08f, 3, 0.85f, 0.65f, 0.55f, 0.5f, 1.0f, 1.01f, 1.0f,
     2.0f, 0.08f, 0.6f, 0.82f, 0.55f},
};

inline constexpr std::size_t kNumFactoryPrograms =
    sizeof(kFactoryPrograms) / sizeof(kFactoryPrograms[0]);

} // namespace dxfm