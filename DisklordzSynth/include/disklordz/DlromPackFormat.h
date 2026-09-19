#pragma once

#include <cstdint>

namespace disklordz::dlrom
{

inline constexpr char kPackMagic[8] = { 'D', 'L', 'R', 'O', 'M', 'P', 'R', '1' };
inline constexpr std::uint32_t kPackVersion = 1;
inline constexpr std::uint32_t kMaxToneLayers = 4;

enum class SampleCategory : std::uint16_t
{
    bell = 0,
    keys = 1,
    pad = 2,
    sub = 3,
};

#pragma pack(push, 1)
struct PackHeader final
{
    char magic[8];
    std::uint32_t version;
    std::uint32_t regionCount;
    std::uint32_t pcmFloatCount;
};

struct SampleRegion final
{
    std::uint8_t toneIndex;
    std::uint8_t rootNote;
    std::uint8_t loKey;
    std::uint8_t hiKey;
    std::uint32_t pcmOffset;
    std::uint32_t frameCount;
    std::uint32_t loopStart;
    std::uint32_t loopEnd;
    std::uint16_t category;
    std::uint16_t reserved;
};
#pragma pack(pop)

static_assert (sizeof (PackHeader) == 20);
static_assert (sizeof (SampleRegion) == 24);

} // namespace disklordz::dlrom
