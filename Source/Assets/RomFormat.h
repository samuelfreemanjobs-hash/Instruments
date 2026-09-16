#pragma once

#include <cstddef>
#include <cstdint>

namespace jdupgraded::assets
{

inline constexpr char kRomMagic[8] = { 'J', 'D', 'U', 'P', 'G', 'R', 'O', 'M' };
inline constexpr std::uint32_t kRomVersion = 2;
inline constexpr std::uint32_t kCleanroomWaveCount = 256;
inline constexpr std::uint32_t kSingleWaveCount = 64;
inline constexpr std::uint32_t kMultisampleSetCount = 24;
inline constexpr std::uint32_t kZonesPerSet = 8;

enum class RomWaveCategory : std::uint16_t
{
    analog = 0,
    digital,
    bell,
    pluck,
    bass,
    noise,
    vocal,
    organ,
    fx
};

enum RomWaveFlags : std::uint16_t
{
    romWaveSingleCycle = 1 << 0,
    romWaveLooped = 1 << 1
};

struct RomHeader final
{
    char magic[8];
    std::uint32_t version;
    std::uint32_t waveCount;
    std::uint32_t waveTableOffset;
    std::uint32_t pcmDataOffset;
    std::uint32_t pcmBytes;
    std::uint32_t crc32;
};

struct RomWaveEntry final
{
    std::uint32_t waveId;
    std::uint16_t numFrames;
    std::uint16_t loopStart;
    std::uint16_t loopEnd;
    std::uint16_t flags;
    std::uint32_t pcmOffsetBytes;
    std::int16_t rootMidiNote;
    std::uint16_t category;
    /** Multisample set ID (0 = standalone wave). */
    std::uint16_t multisampleSetId;
};

inline std::uint32_t crc32 (const std::uint8_t* data, std::size_t size) noexcept
{
    std::uint32_t crc = 0xFFFFFFFFu;
    for (std::size_t i = 0; i < size; ++i)
    {
        crc ^= static_cast<std::uint32_t> (data[i]);
        for (int bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ (0xEDB88320u & (~((crc & 1u) - 1u)));
    }
    return crc ^ 0xFFFFFFFFu;
}

} // namespace jdupgraded::assets
