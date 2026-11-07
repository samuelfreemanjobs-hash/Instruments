#pragma once

#include "Engine/SynthParams.h"

#include <cstddef>
#include <string_view>
#include <vector>

namespace wave9090::presets
{

inline constexpr std::size_t kFactoryPresetCount = 200;

struct FactoryPreset
{
    std::string_view name;
    std::string_view category;
    SynthParams params;
};

const std::vector<FactoryPreset>& getFactoryPresets() noexcept;
std::size_t getFactoryPresetCount() noexcept;
const char* getFactoryPresetCategory (std::size_t index) noexcept;

} // namespace wave9090::presets
