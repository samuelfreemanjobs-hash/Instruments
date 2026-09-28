#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace sp1200
{
struct SliceBoundary
{
    std::int64_t startSample = 0;
    std::int64_t endSample = 0;
};

/** Transient-based slice boundaries within [regionStart, regionEnd). */
std::vector<SliceBoundary> detectTransientSlices (const float* mono,
                                                    std::size_t numSamples,
                                                    std::int64_t regionStart,
                                                    std::int64_t regionEnd,
                                                    int numSlices,
                                                    double sampleRateHz,
                                                    double minGapSeconds = 0.05);

} // namespace sp1200
