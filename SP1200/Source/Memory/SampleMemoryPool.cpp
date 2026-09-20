#include "SampleMemoryPool.h"

#include "../SP1200Constants.h"

namespace sp1200
{
std::optional<std::size_t> SampleMemoryPool::appendSegment (SampleSegment segment)
{
    const auto need = static_cast<std::int64_t> (segment.data.size());
    if (need <= 0 || usedSamples_ + need > kMaxMemorySamples)
        return std::nullopt;

    segments_.push_back (std::move (segment));
    usedSamples_ += need;
    return segments_.size() - 1;
}

const SampleSegment* SampleMemoryPool::getSegment (std::size_t index) const
{
    return index < segments_.size() ? &segments_[index] : nullptr;
}

SampleSegment* SampleMemoryPool::getSegment (std::size_t index)
{
    return index < segments_.size() ? &segments_[index] : nullptr;
}

void SampleMemoryPool::clear()
{
    segments_.clear();
    usedSamples_ = 0;
}

void SampleMemoryPool::replaceAll (std::vector<SampleSegment> segments)
{
    segments_ = std::move (segments);
    recomputeUsed();
}

void SampleMemoryPool::recomputeUsed()
{
    usedSamples_ = 0;
    for (const auto& s : segments_)
        usedSamples_ += static_cast<std::int64_t> (s.data.size());
}

} // namespace sp1200
