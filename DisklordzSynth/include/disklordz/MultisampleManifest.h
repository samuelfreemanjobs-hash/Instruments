#pragma once

#include "disklordz/RomFactorySpec.h"

#include <cstdint>
#include <string>
#include <vector>

namespace disklordz::factory
{

struct SampleZoneSpec final
{
    std::int16_t rootMidiNote = 60;
    std::uint8_t velocityMin = 0;
    std::uint8_t velocityMax = 127;
    std::string wavPath;
    bool looped = false;
};

/**
 * One source multisample (many zones/velocity layers → many WAVs, one DLRROM program).
 * Line-oriented `.manifest` format — see FactoryContent/README.md.
 */
struct MultisampleManifest final
{
    std::string id;
    InstrumentFamily family = InstrumentFamily::acousticPiano;
    RomLayer layer = RomLayer::real;
    SampleTier tier = SampleTier::core;
    std::uint16_t multisampleSetId = 0;
    std::vector<SampleZoneSpec> zones;
};

bool loadMultisampleManifest (const std::string& path, MultisampleManifest& out, std::string& error);

} // namespace disklordz::factory
