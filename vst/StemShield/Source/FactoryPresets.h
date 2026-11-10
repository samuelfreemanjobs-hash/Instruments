#pragma once

#include <array>

struct StemShieldFactoryPreset {
  const char* name;
  float ceilingDbTp;
  float subBalance;     // 0..1
  float crossoverHz;
  float releaseMs;
  float lookaheadMs;
  float inputGainDb;
};

constexpr int kStemShieldFactoryPresetCount = 20;

inline constexpr std::array<StemShieldFactoryPreset, kStemShieldFactoryPresetCount> kStemShieldFactoryPresets = {{
    {"BeatStars Stem Safe", -0.5f, 0.72f, 85.0f, 140.0f, 3.0f, 0.0f},
    {"Airbit Clean Export", -1.0f, 0.65f, 80.0f, 120.0f, 2.5f, 0.0f},
    {"Artist Delivery -0.5", -0.5f, 0.55f, 75.0f, 100.0f, 2.0f, -0.5f},
    {"Loud 808 Bus Tamer", -0.5f, 0.85f, 70.0f, 90.0f, 2.5f, 0.0f},
    {"Streaming -1 dBTP", -1.0f, 0.60f, 82.0f, 150.0f, 3.5f, 0.0f},
    {"Drill Sub Guard", -0.5f, 0.80f, 58.0f, 110.0f, 2.0f, 0.0f},
    {"Phonk Low Contain", -0.5f, 0.78f, 52.0f, 100.0f, 2.5f, 0.5f},
    {"Trap Phone Check", -0.5f, 0.70f, 88.0f, 130.0f, 3.0f, 1.0f},
    {"Master Stem Trim", -0.5f, 0.50f, 90.0f, 180.0f, 4.0f, -1.0f},
    {"Club Peak Shield", -0.3f, 0.75f, 78.0f, 80.0f, 1.5f, 0.0f},
    {"Gentle Sub Balance", -1.5f, 0.45f, 95.0f, 200.0f, 4.0f, 0.0f},
    {"Hard Limit Only", -0.5f, 0.0f, 80.0f, 60.0f, 1.0f, 0.0f},
    {"Wide Lookahead", -0.5f, 0.65f, 80.0f, 160.0f, 6.0f, 0.0f},
    {"Fast Release Punch", -0.5f, 0.68f, 72.0f, 55.0f, 2.0f, 0.0f},
    {"Slow Glue Release", -0.5f, 0.60f, 80.0f, 280.0f, 3.0f, 0.0f},
    {"Hi Sub Cross 110", -0.5f, 0.62f, 110.0f, 120.0f, 2.5f, 0.0f},
    {"Lo Sub Cross 55", -0.5f, 0.82f, 55.0f, 100.0f, 2.0f, 0.0f},
    {"Stems + Headroom", -2.0f, 0.58f, 85.0f, 150.0f, 3.0f, -2.0f},
    {"Aggressive Contain", -0.5f, 1.0f, 68.0f, 85.0f, 2.0f, 0.0f},
    {"Init Safe Export", -0.5f, 0.65f, 80.0f, 120.0f, 2.5f, 0.0f},
}};
