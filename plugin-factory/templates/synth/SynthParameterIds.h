#pragma once

#include <string_view>

namespace SynthParamIDs
{
inline constexpr std::string_view outputGainDb { "outputGainDb" };
inline constexpr std::string_view attack { "attack" };
inline constexpr std::string_view decay { "decay" };
inline constexpr std::string_view sustain { "sustain" };
inline constexpr std::string_view release { "release" };
} // namespace SynthParamIDs
