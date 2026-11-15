#pragma once

#include <array>
#include <cstddef>

namespace play_series {

// Replace with plugin-specific preset table + BinaryData JSON in impl.
struct FactoryPreset {
  const char *name;
  float blend;
  float macros[kMacroCount];
};

inline constexpr std::array<FactoryPreset, kFactoryPresetCount> kFactoryPresets = {{
    {"Init Dual Layer", 0.5f, {0.5f, 0.45f, 0.35f, 0.3f, 0.25f, 0.35f, 0.2f, 0.3f}},
    {"Placeholder 02", 0.6f, {0.55f, 0.5f, 0.4f, 0.35f, 0.3f, 0.4f, 0.25f, 0.35f}},
    {"Placeholder 03", 0.4f, {0.45f, 0.4f, 0.3f, 0.25f, 0.2f, 0.3f, 0.15f, 0.25f}},
    {"Placeholder 04", 0.5f, {0.6f, 0.35f, 0.5f, 0.4f, 0.35f, 0.45f, 0.3f, 0.4f}},
    {"Placeholder 05", 0.35f, {0.4f, 0.55f, 0.25f, 0.3f, 0.45f, 0.2f, 0.35f, 0.5f}},
    {"Placeholder 06", 0.65f, {0.5f, 0.6f, 0.45f, 0.35f, 0.25f, 0.5f, 0.4f, 0.35f}},
    {"Placeholder 07", 0.5f, {0.35f, 0.45f, 0.55f, 0.3f, 0.4f, 0.35f, 0.45f, 0.25f}},
    {"Placeholder 08", 0.55f, {0.5f, 0.5f, 0.4f, 0.45f, 0.3f, 0.55f, 0.35f, 0.4f}},
    {"Placeholder 09", 0.45f, {0.45f, 0.4f, 0.35f, 0.5f, 0.35f, 0.3f, 0.5f, 0.45f}},
    {"Placeholder 10", 0.5f, {0.55f, 0.45f, 0.3f, 0.25f, 0.5f, 0.4f, 0.3f, 0.55f}},
    {"Placeholder 11", 0.6f, {0.4f, 0.5f, 0.45f, 0.35f, 0.25f, 0.45f, 0.25f, 0.35f}},
    {"Placeholder 12", 0.5f, {0.5f, 0.55f, 0.35f, 0.4f, 0.3f, 0.35f, 0.4f, 0.5f}},
}};

} // namespace play_series