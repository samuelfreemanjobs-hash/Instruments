#pragma once

#include "../Memory/SampleMemoryPool.h"

#include <cstdint>
#include <vector>

namespace sp1200
{
std::vector<float> segmentToFloat (const SampleSegment& seg);
bool truncateSegmentInPlace (SampleMemoryPool& pool, std::size_t index, std::int64_t start, std::int64_t end);
bool reverseSegmentInPlace (SampleMemoryPool& pool, std::size_t index);
bool normalizeSegmentInPlace (SampleMemoryPool& pool, std::size_t index);

} // namespace sp1200
