#pragma once

#include <cstdint>
#include <string>

namespace disklordz::content
{

/** Playable layer referencing a multisample program on a ROM card. */
struct ToneRecord final
{
    std::string id;
    std::string multisampleId;
    std::uint8_t romBank = 0;
    std::uint16_t program = 1;
    float defaultLevel = 1.0f;
};

} // namespace disklordz::content
