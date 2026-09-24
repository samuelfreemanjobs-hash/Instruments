#pragma once

#include "Engine/SynthParams.h"

#include <string>
#include <vector>

namespace prophetrev2::presets
{

inline constexpr int kFactoryPresetCount = 1028;
inline constexpr int kFoundationPresetCount = 47;
inline constexpr int kVariationPresetCount = 981;

struct FactoryPreset
{
    std::string name;
    std::string category;
    SynthParams params;
    int foundationIndex = -1; // 0..20 for foundations; 0..20 for variation parent
    bool isFoundation = false;
};

/** Full factory bank (lazy-built once): 21 foundations + 1,007 deterministic variations. */
const std::vector<FactoryPreset>& getFactoryPresets() noexcept;

} // namespace prophetrev2::presets
