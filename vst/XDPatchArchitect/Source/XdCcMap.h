#pragma once

#include <array>
#include <cstdint>

namespace xd {

struct CcBinding {
  const char *paramId;
  std::uint8_t cc;
};

inline constexpr std::array<CcBinding, 9> kCcMap{{
    {"filter_cutoff", 43},
    {"filter_resonance", 44},
    {"amp_attack", 73},
    {"amp_decay", 75},
    {"amp_sustain", 76},
    {"amp_release", 72},
    {"lfo_rate", 20},
    {"lfo_depth", 21},
    {"master_volume", 7},
}};

inline std::uint8_t normToCc(float norm) {
  const auto clamped = norm < 0.f ? 0.f : (norm > 1.f ? 1.f : norm);
  return static_cast<std::uint8_t>(clamped * 127.f + 0.5f);
}

} // namespace xd
