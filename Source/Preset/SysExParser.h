#pragma once

// Phase 3: Roland JD SysEx ingest with checksum verification (no ROM in installer).

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

class SysExParser final
{
public:
    static std::optional<SysExParseResult> feed (const std::uint8_t* data, std::size_t size) noexcept
    {
        if (data == nullptr || size < 4)
            return std::nullopt;

        // Roland manufacturer ID 0x41 — full packet dispatch in phase 3.
        SysExParseResult result;
        result.checksumValid = false;
        result.bytesConsumed = size;
        return result;
    }
};

} // namespace jdupgraded::preset
