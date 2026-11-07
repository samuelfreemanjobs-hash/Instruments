#pragma once

#include "Engine/SynthParams.h"

#include <string>
#include <vector>

namespace pluggva::presets
{

struct FactoryPreset
{
    std::string name;
    std::string category;
    SynthParams params;
};

const std::vector<FactoryPreset>& getFactoryPresets() noexcept;

} // namespace pluggva::presets
