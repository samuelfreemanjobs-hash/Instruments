#pragma once

#include "Engine/SynthParams.h"

#include <string_view>
#include <vector>

namespace wave9090::presets
{

struct FactoryPreset
{
    std::string_view name;
    std::string_view category;
    SynthParams params;
};

const std::vector<FactoryPreset>& getFactoryPresets() noexcept;

/** UI section title for preset browser grouping. */
std::string_view categoryUiHeading (std::string_view category) noexcept;

} // namespace wave9090::presets
