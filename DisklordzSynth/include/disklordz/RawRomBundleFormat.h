#pragma once

#include <cstdint>

namespace disklordz::rawrom
{

inline constexpr char kRomBundleMagic[8] = { 'D', 'L', 'R', 'C', 'A', 'T', '0', '1' };
inline constexpr std::uint32_t kRomBundleVersion = 1;
inline constexpr std::uint32_t kRomBundleNameBytes = 32;

#pragma pack(push, 1)
struct RomBundleHeader final
{
    char magic[8];
    std::uint32_t version;
    std::uint32_t fileCount;
};

struct RomBundleEntry final
{
    char name[kRomBundleNameBytes];
    std::uint32_t offset;
    std::uint32_t size;
};
#pragma pack(pop)

} // namespace disklordz::rawrom
