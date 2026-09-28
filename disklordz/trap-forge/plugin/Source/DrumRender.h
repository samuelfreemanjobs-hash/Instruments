#pragma once

#include "TrapForgePreset.h"

#include <vector>

namespace trapforge
{

/** Offline render one hit (mono) at `sampleRate` — layered kick / 808 glide / snare noise. */
std::vector<float> renderDrumMono (const TrapForgePreset& preset, double sampleRate, float velocity = 1.0f);

} // namespace trapforge
