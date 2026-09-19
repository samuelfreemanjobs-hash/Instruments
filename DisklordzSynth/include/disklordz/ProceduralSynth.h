#pragma once

#include <cstddef>
#include <vector>

namespace disklordz::synth
{

/** Clean-room procedural renderers (sellable sample sources). */

void normalizePeak (std::vector<float>& buffer, float targetPeak = 0.92f);

std::vector<float> renderBell (std::size_t frames, float inharmonicity, float decay);
std::vector<float> renderKeys (std::size_t frames, float decay, float brightness);
std::vector<float> renderPad (std::size_t frames, float lpCoeff, bool deterministicSeed = false);
std::vector<float> renderSub808 (std::size_t frames, float punch);

enum class EngineId
{
    bell,
    keys,
    pad,
    sub808
};

const char* engineIdName (EngineId id);

/** Single-shot render dispatch for CLI / batch tools. */
std::vector<float> renderEngine (EngineId id, std::size_t frames, float paramA, float paramB);

} // namespace disklordz::synth
