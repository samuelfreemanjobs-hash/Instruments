#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

namespace jdupgraded::preset
{

struct SysExParseResult
{
    bool checksumValid = false;
    std::size_t bytesConsumed = 0;
};

/** Validates Roland SysEx framing and checksum (0x41 manufacturer). No ROM data imported. */
class SysExParser final
{
public:
    static std::optional<SysExParseResult> feed (const std::uint8_t* data, std::size_t size) noexcept;
};

} // namespace jdupgraded::preset
