#pragma once

#include "../SP1200Constants.h"

namespace sp1200
{
struct MultiPitchState
{
    bool enabled = false;
    int sourceSegmentIndex = -1;
    std::array<float, kMultiPitchSlots> semitoneOffsets {};
};

struct PadAssignment
{
    int segmentIndex = -1;
    float tuneSemitones = 0.0f;
    float level = 1.0f;
    float decay = 1.0f;
};

inline MultiPitchState defaultMultiPitchState()
{
    MultiPitchState s;
    for (int i = 0; i < kMultiPitchSlots; ++i)
        s.semitoneOffsets[static_cast<std::size_t> (i)] = kDefaultMultiPitchOffsets[i];
    return s;
}

struct PadBank
{
    PadAssignment pads[kNumPads] {};
};

} // namespace sp1200
