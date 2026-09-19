#pragma once

#include "Engine/RomplerParams.h"

#include <string_view>
#include <vector>

namespace disklordz::rompler::presets
{

struct FactoryPreset final
{
    std::string_view name;
    std::string_view category;
    RomplerParams params;
};

const std::vector<FactoryPreset>& getFactoryPresets() noexcept;

} // namespace disklordz::rompler::presets
