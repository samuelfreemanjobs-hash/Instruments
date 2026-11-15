#pragma once

#include <cmath>

namespace play_series {

inline float equalPowerBlendA(float blend01) noexcept {
  return std::cos(blend01 * 0.5f * 3.14159265358979323846f);
}

inline float equalPowerBlendB(float blend01) noexcept {
  return std::sin(blend01 * 0.5f * 3.14159265358979323846f);
}

// APVTS IDs shared by trilogy WebView shell (see plugins/PLAY_SYNTH_SHARED_UI.md).
inline constexpr const char *kLayerBlend = "layer_blend";
inline constexpr const char *kXyX = "xy_x";
inline constexpr const char *kXyY = "xy_y";
inline constexpr const char *kDiceTrigger = "dice_trigger";
inline constexpr const char *kMasterLevel = "master_level";

inline constexpr int kVoiceCount = 12;
inline constexpr int kMacroCount = 8;
inline constexpr int kFactoryPresetCount = 12;

} // namespace play_series