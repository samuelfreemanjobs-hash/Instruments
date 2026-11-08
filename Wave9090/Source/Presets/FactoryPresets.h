#pragma once

#include "Engine/SynthParams.h"

#include <string>
#include <vector>

namespace wave9090::presets
{

struct FactoryPreset
{
    std::string name;
    std::string category;
    SynthParams params;
};

const std::vector<FactoryPreset>& getFactoryPresets() noexcept;

} // namespace wave9090::presets
