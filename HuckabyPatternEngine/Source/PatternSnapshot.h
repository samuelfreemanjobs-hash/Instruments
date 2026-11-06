#pragma once

#include "TrackModel.h"
#include <JuceHeader.h>
#include <array>
#include <cstdint>

namespace huckaby
{

struct PatternSnapshot final
{
    uint64_t bits { 0 };
    std::array<float, kMaxSteps> velocities {};
};

} // namespace huckaby
