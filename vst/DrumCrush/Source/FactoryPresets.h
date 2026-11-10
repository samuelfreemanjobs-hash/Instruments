#pragma once

#include <array>

struct FactoryPreset {
  const char* name;
  float punch;   // 0..1
  float crush;   // 0..1
  float clip;    // 0..1
  float mix;     // 0..1
  float outputDb;
};

constexpr int kFactoryPresetCount = 20;

inline constexpr std::array<FactoryPreset, kFactoryPresetCount> kFactoryPresets = {{
    // Trap knock (5)
    {"Atlanta Knock", 0.62f, 0.55f, 0.60f, 1.0f, 0.0f},
    {"Hard Trap Bus", 0.70f, 0.62f, 0.68f, 1.0f, -0.5f},
    {"Slide Kit Punch", 0.58f, 0.48f, 0.55f, 1.0f, 0.0f},
    {"808 Bus Glue", 0.45f, 0.40f, 0.52f, 0.92f, 0.0f},
    {"Metro Punch", 0.65f, 0.58f, 0.62f, 1.0f, -1.0f},
    // Drill transient (5)
    {"UK Drill Snap", 0.72f, 0.50f, 0.58f, 1.0f, 0.0f},
    {"Brixton Crush", 0.68f, 0.65f, 0.70f, 1.0f, -0.5f},
    {"Dark Drill Hit", 0.75f, 0.55f, 0.65f, 1.0f, 0.0f},
    {"Rim Shot Cut", 0.80f, 0.45f, 0.50f, 1.0f, 0.5f},
    {"NY Drill Bus", 0.66f, 0.60f, 0.64f, 0.98f, 0.0f},
    // Parallel blend (5)
    {"Parallel 50", 0.55f, 0.50f, 0.55f, 0.50f, 0.0f},
    {"NY Compression", 0.48f, 0.42f, 0.48f, 0.45f, 0.0f},
    {"Sub Safe Punch", 0.40f, 0.35f, 0.45f, 0.65f, 0.0f},
    {"Stacked Snares", 0.62f, 0.52f, 0.58f, 0.55f, -0.5f},
    {"Ghost Note Lift", 0.52f, 0.38f, 0.42f, 0.60f, 0.5f},
    // Clip & ceiling (5)
    {"Brickwall Club", 0.55f, 0.70f, 0.85f, 1.0f, -1.5f},
    {"Limiter Slam", 0.50f, 0.65f, 0.90f, 1.0f, -2.0f},
    {"Tape Clip Bus", 0.42f, 0.58f, 0.75f, 1.0f, 0.0f},
    {"Stream Loud", 0.48f, 0.55f, 0.80f, 1.0f, 1.0f},
    {"Clean Ceiling", 0.35f, 0.30f, 0.60f, 0.85f, -0.5f},
}};
