#include "SysExParser.h"

namespace jdupgraded::preset
{

namespace
{

bool isRolandManufacturer (const std::uint8_t* data, std::size_t size) noexcept
{
    return size >= 3 && data[0] == 0xF0 && data[1] == 0x41;
}

std::uint8_t rolandChecksum (const std::uint8_t* data, std::size_t start, std::size_t endExclusive) noexcept
{
    std::uint32_t sum = 0;
    for (std::size_t i = start; i < endExclusive; ++i)
        sum += data[i];
    return static_cast<std::uint8_t> ((128 - (sum & 0x7F)) & 0x7F);
}

} // namespace

std::optional<SysExParseResult> SysExParser::feed (const std::uint8_t* data, std::size_t size) noexcept
{
    if (data == nullptr || size < 5 || data[0] != 0xF0)
        return std::nullopt;

    std::size_t end = size;
    for (std::size_t i = 1; i < size; ++i)
    {
        if (data[i] == 0xF7)
        {
            end = i + 1;
            break;
        }
    }

    SysExParseResult result;
    result.bytesConsumed = end;

    if (! isRolandManufacturer (data, end) || end < 6)
    {
        result.checksumValid = false;
        return result;
    }

    const std::uint8_t expected = rolandChecksum (data, 0, end - 2);
    const std::uint8_t received = data[end - 2];
    result.checksumValid = expected == received;
    return result;
}

} // namespace jdupgraded::preset
