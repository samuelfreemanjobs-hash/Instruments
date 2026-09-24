#pragma once

#include "Engine/SynthParams.h"
#include "Presets/FactoryPresets.h"

#include <array>
#include <string_view>

namespace prophetrev2::presets
{

struct FoundationPatch
{
    std::string_view name;
    std::string_view category;
    SynthParams params;
};

const std::array<FoundationPatch, kFoundationPresetCount>& getFoundationPresets() noexcept;

} // namespace prophetrev2::presets
