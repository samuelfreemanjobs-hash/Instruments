#pragma once

#include <cstdint>

// Chassis tokens for a hardware-style drum editor. Layout code reads these
// and sizes controls from getLocalBounds(); it does not hard-code pixel origins.
namespace dlz::ui {

inline constexpr std::uint32_t chassis = 0xFF1A1C1Eu;
inline constexpr std::uint32_t bezel = 0xFF2A2E32u;
inline constexpr std::uint32_t ink = 0xFFE6E1D6u;
inline constexpr std::uint32_t accent = 0xFFD2A15Au;
inline constexpr std::uint32_t shadow = 0xFF0B0C0Du;

inline constexpr int knobFrames = 128;
inline constexpr int designWidth = 900;
inline constexpr int designHeight = 560;

inline constexpr int scalePercent(int percent) noexcept {
    return percent;
}

} // namespace dlz::ui
