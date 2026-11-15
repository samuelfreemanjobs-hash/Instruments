#pragma once

#include <array>

namespace mpc_play {

struct FactoryProgram {
  const char *name;
  float blend;
  float k1;
  float k2;
  float k3;
};

inline constexpr int kProgramCount = 8;
inline constexpr int kVoiceCount = 8;

inline constexpr std::array<FactoryProgram, kProgramCount> kPrograms = {{
    {"Init Dual", 0.5f, 0.5f, 0.5f, 0.5f},
    {"Play 02", 0.55f, 0.45f, 0.4f, 0.35f},
    {"Play 03", 0.4f, 0.6f, 0.3f, 0.25f},
    {"Play 04", 0.5f, 0.35f, 0.55f, 0.4f},
    {"Play 05", 0.6f, 0.5f, 0.45f, 0.35f},
    {"Play 06", 0.45f, 0.55f, 0.5f, 0.3f},
    {"Play 07", 0.5f, 0.4f, 0.6f, 0.45f},
    {"Play 08", 0.55f, 0.5f, 0.35f, 0.5f},
}};

} // namespace mpc_play