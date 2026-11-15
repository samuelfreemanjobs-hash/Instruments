#pragma once

namespace tr808909 {
struct Program {
  const char *name;
  float kickDecay, kickStart, kickEnd, kickSweep;
  float snareTone, snareNoise, snareDecay;
  float hatDecay, hatNoise, drive;
};
inline constexpr Program kPrograms[] = {
    {"808 Classic", 0.55f, 220.0f, 48.0f, 0.12f, 180.0f, 0.72f, 0.22f, 0.08f,
     0.85f, 0.15f},
    {"909 House", 0.42f, 180.0f, 40.0f, 0.08f, 210.0f, 0.65f, 0.18f, 0.06f,
     0.9f, 0.28f},
    {"Hybrid Punch", 0.65f, 240.0f, 42.0f, 0.15f, 195.0f, 0.78f, 0.25f,
     0.1f, 0.88f, 0.22f},
    {"Kick Sub", 0.85f, 120.0f, 32.0f, 0.2f, 160.0f, 0.6f, 0.2f, 0.05f,
     0.7f, 0.1f},
};
inline constexpr int kNumPrograms = 4;
} // namespace tr808909