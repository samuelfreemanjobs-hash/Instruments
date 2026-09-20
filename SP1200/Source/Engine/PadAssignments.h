#pragma once

#include "../SP1200Constants.h"

#include <array>

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
    int chokeGroup = kNoChokeGroup;
};

struct PadBank
{
    PadAssignment pads[kNumPads] {};
};

inline void assignDefaultPadChokeGroups (PadBank& bank)
{
    for (int p = 0; p < kNumPads; ++p)
    {
        const int ch = p % 8;
        if (ch == 4 || ch == 5)
            bank.pads[p].chokeGroup = kDefaultHatChokeGroup;
    }
}

inline MultiPitchState defaultMultiPitchState()
{
    MultiPitchState s;
    for (int i = 0; i < kMultiPitchSlots; ++i)
        s.semitoneOffsets[static_cast<std::size_t> (i)] = kDefaultMultiPitchOffsets[i];
    return s;
}

} // namespace sp1200
