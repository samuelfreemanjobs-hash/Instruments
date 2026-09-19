#pragma once

#include "disklordz/EngineParams.h"

#include <cstdint>

namespace disklordz::rawrom
{

inline constexpr char kRomMagic[8] = { 'D', 'L', 'R', 'R', 'O', 'M', '0', '1' };
inline constexpr std::uint32_t kRomVersion1 = 1;
inline constexpr std::uint32_t kRomVersion = 2;

/** Four performance layers in DISKLORDZ ROMPLER (not the same as multisample program count). */
inline constexpr std::uint32_t kRomplerToneLayers = 4;

/** Max multisample program id indexed per `.dlrrom` file. */
inline constexpr std::uint32_t kMaxMultisampleSetsPerRom = 128;

inline constexpr std::uint32_t kZonesPerSet = 8; // legacy v1 factory

/** Workstation-scale factory library (Fantom / Triton / Motif style multi-card layout). */
inline constexpr std::uint32_t kFactoryRomBankCount = 16;
inline constexpr std::uint32_t kProgramsPerEngine = 8;
inline constexpr std::uint32_t kProgramsPerRomBank = kProgramsPerEngine * 4;
inline constexpr std::uint32_t kZonesPerProgram = 12;
inline constexpr std::uint32_t kStandaloneWavesPerBank = 64;

enum WaveFlags : std::uint16_t
{
    waveLooped = 1 << 0
};

enum class RomBankCategory : std::uint16_t
{
    studioPop = 0,
    analogSynth = 1,
    digitalWave = 2,
    acousticPlucked = 3,
    bassAndSub = 4,
    bellAndMallet = 5,
    padAndString = 6,
    leadAndSolo = 7,
    fxAndTexture = 8,
    trapAndPhonk = 9,
    vintageKeys = 10,
    orchestralLite = 11,
    worldAndEthnic = 12,
    drumAndPerc = 13,
    cinematic = 14,
    expansion = 15
};

#pragma pack(push, 1)
struct RomHeader final
{
    char magic[8];
    std::uint32_t version;
    /** Expansion card index (0…N-1), like SRX / EXB slots. */
    std::uint32_t bankIndex;
    std::uint16_t bankCategory;
    std::uint16_t reserved;
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
    std::uint16_t engineId;
    /** 1…128 multisample program in this ROM file; 0 = standalone wave. */
    std::uint16_t multisampleSetId;
};
#pragma pack(pop)

static_assert (sizeof (RomHeader) == 28);
static_assert (sizeof (WaveEntry) == 28);

/** @deprecated Use kRomplerToneLayers — kept for older includes. */
inline constexpr std::uint32_t kMaxMultisampleSets = kRomplerToneLayers;

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

/** Program slot 0…7 within each engine family on a ROM card. */
inline std::uint16_t multisampleSetForToneProgram (std::uint8_t toneIndex, std::uint8_t programVariant)
{
    const std::uint16_t variant = static_cast<std::uint16_t> (programVariant % kProgramsPerEngine);
    switch (toneIndex % kRomplerToneLayers)
    {
        case 0: return static_cast<std::uint16_t> (1 + variant);
        case 1: return static_cast<std::uint16_t> (1 + kProgramsPerEngine + variant);
        case 2: return static_cast<std::uint16_t> (1 + 2 * kProgramsPerEngine + variant);
        case 3: return static_cast<std::uint16_t> (1 + 3 * kProgramsPerEngine + variant);
    }
    return 1;
}

inline RomBankCategory categoryForFactoryBank (std::uint32_t bankIndex)
{
    return static_cast<RomBankCategory> (bankIndex % kFactoryRomBankCount);
}

} // namespace disklordz::rawrom
