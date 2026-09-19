#pragma once

#include "disklordz/EngineParams.h"

#include <cstddef>
#include <vector>

namespace disklordz::synth
{

void normalizePeak (std::vector<float>& buffer, float targetPeak = 0.92f);

std::vector<float> renderSubtractive (std::size_t frames, const EngineParams& p);
std::vector<float> renderAdditive (std::size_t frames, const EngineParams& p);
std::vector<float> renderKarplus (std::size_t frames, const EngineParams& p, bool deterministicNoise = false);
std::vector<float> renderWave (std::size_t frames, const EngineParams& p);

std::vector<float> renderEngine (EngineId id, std::size_t frames, const EngineParams& p);

/** Convenience: two-parameter API used by CLI. */
std::vector<float> renderEngine (EngineId id, std::size_t frames, float paramA, float paramB);

} // namespace disklordz::synth
