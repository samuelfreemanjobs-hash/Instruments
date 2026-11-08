#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <string>

namespace wave9090::presets::agent
{

inline constexpr std::size_t kFactoryPresetCount = 256;

inline float rand01 (std::uint32_t& state) noexcept
{
    state = state * 1664525u + 1013904223u;
    return static_cast<float> (state >> 8) * (1.0f / 16777216.0f);
}

template <typename T>
T clamp (T value, T lo, T hi) noexcept
{
    return std::min (hi, std::max (lo, value));
}

} // namespace wave9090::presets::agent
