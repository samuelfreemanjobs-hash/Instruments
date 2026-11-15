#pragma once

#include <array>

struct FactoryPreset {
  const char* name;
  float drive;     // 0..1
  float wow;       // 0..1
  float flutter;   // 0..1
  float age;       // 0..1
  float noise;     // 0..1
  float mix;       // 0..1
  float outputDb;  // dB
};

constexpr int kFactoryPresetCount = 20;

inline constexpr std::array<FactoryPreset, kFactoryPresetCount> kFactoryPresets = {{
    // Drift phonk (5)
    {"Drift Memphis", 0.58f, 0.42f, 0.55f, 0.38f, 0.22f, 1.0f, 0.0f},
    {"Cowbell Cassette", 0.72f, 0.35f, 0.62f, 0.32f, 0.18f, 1.0f, -0.5f},
    {"Highway Drift", 0.65f, 0.50f, 0.48f, 0.45f, 0.28f, 1.0f, 0.0f},
    {"Night Ride", 0.55f, 0.55f, 0.40f, 0.50f, 0.35f, 0.95f, 0.0f},
    {"SK8 Tape", 0.68f, 0.48f, 0.58f, 0.42f, 0.25f, 1.0f, -1.0f},
    // Memphis grit (5)
    {"Dungeon 90s", 0.75f, 0.38f, 0.45f, 0.55f, 0.30f, 1.0f, 0.0f},
    {"Tape Sludge", 0.80f, 0.45f, 0.52f, 0.60f, 0.40f, 1.0f, 0.5f},
    {"VHS Booth", 0.62f, 0.52f, 0.38f, 0.48f, 0.32f, 1.0f, 0.0f},
    {"Graveyard Loop", 0.70f, 0.30f, 0.35f, 0.62f, 0.28f, 0.98f, -0.5f},
    {"Lo-Rider Memphis", 0.78f, 0.40f, 0.50f, 0.58f, 0.22f, 1.0f, 0.0f},
    // Lofi chill (5)
    {"Rainy Study", 0.35f, 0.28f, 0.22f, 0.65f, 0.45f, 0.88f, 0.0f},
    {"Dusty Chamber", 0.42f, 0.32f, 0.25f, 0.72f, 0.38f, 0.85f, -1.0f},
    {"Warm Bias", 0.38f, 0.20f, 0.18f, 0.55f, 0.30f, 0.80f, 0.0f},
    {"Faded Mixtape", 0.48f, 0.35f, 0.30f, 0.68f, 0.42f, 0.90f, 0.0f},
    {"Porch Swing", 0.30f, 0.25f, 0.15f, 0.50f, 0.25f, 0.75f, 0.5f},
    // Aggressive / mix (5)
    {"Flutter Punch", 0.85f, 0.25f, 0.85f, 0.35f, 0.15f, 1.0f, 0.0f},
    {"Wow Max", 0.60f, 0.95f, 0.70f, 0.40f, 0.20f, 1.0f, -0.5f},
    {"Noise Floor", 0.50f, 0.40f, 0.45f, 0.55f, 0.75f, 1.0f, 0.0f},
    {"Parallel Dirt", 0.55f, 0.30f, 0.35f, 0.45f, 0.18f, 0.55f, 0.0f},
    {"Master Trim Safe", 0.40f, 0.22f, 0.28f, 0.38f, 0.12f, 0.70f, -2.0f},
}};
