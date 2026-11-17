#pragma once

#include "PlaySeriesCommon.h"
#include <algorithm>
#include <array>
#include <cmath>

namespace play_series {

struct MacroTargets {
  float filterCutoffHz = 1200.0f;
  float filterResonance = 0.35f;
  float layerABlend = 0.5f;
  float layerBBoost = 1.0f;
  float layerABrightness = 0.5f;
  float layerBWavetable = 0.35f;
  float layerBAir = 0.25f;
  float subLevel = 0.4f;
  float masterDirt = 0.0f;
  float spaceSend = 0.25f;
  float delaySend = 0.15f;
};

class MacroRouter {
public:
  MacroTargets compute(const std::array<float, kMacroCount> &macros, float xyY,
                       float baseCutoffHz) const noexcept {
    MacroTargets t;
    t.layerABlend = macros[0];
    t.layerABrightness = macros[1];
    t.layerBWavetable = macros[2];
    t.layerBAir = macros[3];
    t.subLevel = macros[4];
    t.masterDirt = macros[5];
    t.filterResonance = 0.2f + macros[6] * 0.75f;
    t.spaceSend = macros[7];

    const float toneMix = std::clamp(0.35f * macros[0] + 0.65f * xyY, 0.0f, 1.0f);
    const float minHz = 80.0f;
    const float maxHz = 12000.0f;
    t.filterCutoffHz = minHz * std::pow(maxHz / minHz, toneMix) * (0.85f + 0.15f * baseCutoffHz / 1200.0f);
    t.layerBBoost = 0.6f + 0.8f * macros[3];
    t.delaySend = macros[7] * 0.65f;
    return t;
  }
};

} // namespace play_series