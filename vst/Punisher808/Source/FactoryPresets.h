#pragma once

#include <array>

struct FactoryPreset {
  const char* name;
  float punish;       // 0..1
  float shape;        // 0..1  (0 = warm 2nd, 1 = 3rd bite)
  float crossoverHz; // 40..120
  float mix;          // 0..1
  float outputDb;     // dB
};

constexpr int kFactoryPresetCount = 20;

inline constexpr std::array<FactoryPreset, kFactoryPresetCount> kFactoryPresets = {{
    // UK Drill Slides (5)
    {"Slide Punish 808", 0.62f, 0.78f, 58.0f, 1.0f, 0.0f},
    {"Brixton Slap", 0.55f, 0.82f, 62.0f, 1.0f, -0.5f},
    {"Dark Drill Sub", 0.68f, 0.75f, 55.0f, 1.0f, 0.0f},
    {"Roller Slide", 0.50f, 0.70f, 65.0f, 0.92f, 0.0f},
    {"15 Slide Max", 0.72f, 0.85f, 52.0f, 1.0f, -1.0f},
    // Phonk Memphis Grit (5)
    {"Memphis Grit", 0.75f, 0.55f, 50.0f, 1.0f, 0.0f},
    {"Cowbell Sub", 0.80f, 0.48f, 48.0f, 1.0f, -0.5f},
    {"Dungeon Punch", 0.70f, 0.60f, 55.0f, 1.0f, 0.5f},
    {"Tape Sludge", 0.65f, 0.52f, 58.0f, 0.95f, 0.0f},
    {"Dante Memphis", 0.78f, 0.45f, 52.0f, 1.0f, 0.0f},
    // Trap Mobile Boost (5)
    {"Phone Check", 0.50f, 0.65f, 72.0f, 1.0f, 1.0f},
    {"IG Reel 808", 0.48f, 0.62f, 68.0f, 1.0f, 1.5f},
    {"Airdrop Boost", 0.52f, 0.68f, 75.0f, 1.0f, 0.5f},
    {"Stream Safe Sub", 0.45f, 0.70f, 70.0f, 0.98f, 1.0f},
    {"Car Test", 0.42f, 0.58f, 80.0f, 1.0f, 2.0f},
    // Clean Sub Tamer (5)
    {"Clean Sub Tamer", 0.28f, 0.35f, 60.0f, 0.85f, 0.0f},
    {"Phase Safe", 0.22f, 0.30f, 55.0f, 0.80f, 0.0f},
    {"Club Sub Guard", 0.35f, 0.40f, 58.0f, 0.88f, -1.0f},
    {"Master 808 Trim", 0.30f, 0.45f, 62.0f, 0.75f, -0.5f},
    {"Low Headroom", 0.18f, 0.25f, 65.0f, 0.70f, -1.5f},
}};
