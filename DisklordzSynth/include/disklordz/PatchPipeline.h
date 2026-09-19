#pragma once

#include "disklordz/RomFactorySpec.h"

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace disklordz::factory
{

/**
 * One playable tone: pointer into ROM (card + multisample program + mix).
 * Many factory presets share the same Tone with different macros/FX naming.
 */
struct ToneDescriptor final
{
    std::uint8_t romBank = 0;
    std::uint16_t multisampleSetId = 1;
    std::uint8_t programVariant = 0;
    float level = 1.0f;
};

/** Four-tone patch (performance layer). */
struct PatchDescriptor final
{
    std::string id;
    std::string displayName;
    InstrumentFamily browserFamily = InstrumentFamily::pad;
    RomLayer primaryLayer = RomLayer::cyberShift;
    std::array<ToneDescriptor, 4> tones{};
};

/**
 * Factory preset = patch + macro snapshot (+ optional genre tag).
 * Target catalog: 1,000+ entries referencing ~250–350 source multisamples.
 */
struct PresetCatalogEntry final
{
    std::string presetName;
    std::string category;
    std::string sourceMultisampleId;
    PatchDescriptor patch;
};

} // namespace disklordz::factory
