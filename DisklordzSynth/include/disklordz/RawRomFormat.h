#pragma once

#include "disklordz/EngineParams.h"

#include <cstdint>

namespace disklordz::rawrom
{

inline constexpr char kRomMagic[8] = { 'D', 'L', 'R', 'R', 'O', 'M', '0', '1' };
inline constexpr std::uint32_t kRomVersion = 1;
inline constexpr std::uint32_t kMaxMultisampleSets = 4;
inline constexpr std::uint32_t kZonesPerSet = 8;

enum WaveFlags : std::uint16_t
{
    waveLooped = 1 << 0
};

#pragma pack(push, 1)
struct RomHeader final
{
    char magic[8];
    std::uint32_t version;
    std::uint32_t waveCount;
    std::uint32_t pcmFloatCount;
};

struct WaveEntry final
{
    std::uint32_t waveId;
    std::uint32_t pcmOffset;
    std::uint32_t numFrames;
    std::uint32_t loopStart;
    std::uint32_t loopEnd;
    std::int16_t rootMidiNote;
    std::uint16_t flags;
    /** Engine that synthesized this wave (for tooling / future presets). */
    std::uint16_t engineId;
    /** 1…4 = multisample program per engine; 0 = standalone wave in ROM. */
    std::uint16_t multisampleSetId;
};
#pragma pack(pop)

static_assert (sizeof (RomHeader) == 20);
static_assert (sizeof (WaveEntry) == 28);

/** Multisample set ID for each rompler tone layer (engine-owned ROM bank). */
inline std::uint16_t multisampleSetForEngine (synth::EngineId engine)
{
    switch (engine)
    {
        case synth::EngineId::additive: return 1;
        case synth::EngineId::karplus: return 2;
        case synth::EngineId::wave: return 3;
        case synth::EngineId::subtractive: return 4;
    }
    return 0;
}

} // namespace disklordz::rawrom
