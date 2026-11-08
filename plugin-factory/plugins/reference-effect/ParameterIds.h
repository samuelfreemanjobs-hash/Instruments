#pragma once

#include <string_view>

namespace ParamIDs
{
inline constexpr std::string_view cutoffHz { "cutoffHz" };
inline constexpr std::string_view resonance { "resonance" };
inline constexpr std::string_view drive { "drive" };
inline constexpr std::string_view lfoRateHz { "lfoRateHz" };
inline constexpr std::string_view lfoDepth { "lfoDepth" };
inline constexpr std::string_view mix { "mix" };
inline constexpr std::string_view outputGainDb { "outputGainDb" };
} // namespace ParamIDs
