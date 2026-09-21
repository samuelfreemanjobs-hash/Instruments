#pragma once

#include "../Memory/SampleMemoryPool.h"
#include "PadAssignments.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace sp1200
{
std::vector<float> segmentToFloat (const SampleSegment& seg);
bool truncateSegmentInPlace (SampleMemoryPool& pool, std::size_t index, std::int64_t start, std::int64_t end);
bool reverseSegmentInPlace (SampleMemoryPool& pool, std::size_t index);
bool normalizeSegmentInPlace (SampleMemoryPool& pool, std::size_t index);

/** Append segment B onto A in place; remove B; remap pad segment indices. */
bool combineSegments (SampleMemoryPool& pool, PadBank& pads, std::size_t indexA, std::size_t indexB);

bool setSegmentBank (SampleMemoryPool& pool, std::size_t index, int bankIndex);

void remapPadSegmentIndex (PadBank& pads, std::size_t removedIndex, int replacementIndex);

} // namespace sp1200
