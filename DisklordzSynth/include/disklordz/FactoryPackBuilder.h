#pragma once

#include "disklordz/DlromPackFormat.h"

#include <cstdint>
#include <string>
#include <vector>

namespace disklordz::synth
{

struct PackRegionSpec final
{
    dlrom::SampleCategory category;
    std::uint8_t toneIndex;
    std::uint8_t root;
    std::uint8_t lo;
    std::uint8_t hi;
    std::vector<float> pcm;
    std::uint32_t loopStart = 0;
    std::uint32_t loopEnd = 0;
};

/** Default 32-region factory map (8 zones × 4 tones) for DISKLORDZ ROMPLER. */
void buildRomplerFactoryRegions (std::vector<PackRegionSpec>& out);

bool writeDlromPack (const std::string& path, const std::vector<PackRegionSpec>& specs);

} // namespace disklordz::synth
