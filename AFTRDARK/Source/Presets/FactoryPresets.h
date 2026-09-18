#pragma once

#include "Engine/SynthParams.h"

#include <string_view>
#include <vector>

namespace aftrdark::presets
{

struct FactoryPreset
{
    std::string_view name;
    std::string_view category;
    SynthParams params;
};

const std::vector<FactoryPreset>& getFactoryPresets() noexcept;

} // namespace aftrdark::presets
