#pragma once

#include "../SP1200Constants.h"

namespace sp1200
{
struct PadAssignment
{
    int segmentIndex = -1;
    float tuneSemitones = 0.0f;
    float level = 1.0f;
    float decay = 1.0f; // placeholder for envelope length scale
};

struct PadBank
{
    PadAssignment pads[kNumPads] {};
};

} // namespace sp1200
