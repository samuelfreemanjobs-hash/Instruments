#pragma once

#include <cstdint>

namespace rev2trap::presets
{

struct SynthParams
{
    float oscMix = 0.55f;
    float osc2DetuneCents = 7.0f;
    float subLevel = 0.35f;
    float cutoff = 0.55f;
    float resonance = 0.28f;
    float filterEnv = 0.5f;
    float drive = 1.0f;
    float ampAttack = 0.008f;
    float ampDecay = 0.35f;
    float ampSustain = 0.55f;
    float ampRelease = 0.45f;
    float filtAttack = 0.005f;
    float filtDecay = 0.4f;
    float filtSustain = 0.25f;
    float filtRelease = 0.5f;
};

/** 128 procedural trap presets (Jeezy / Shawty Redd / Gucci / neutral lanes). */
SynthParams getFactoryPreset (int programIndex) noexcept;

constexpr int kNumFactoryPresets = 128;

} // namespace rev2trap::presets
