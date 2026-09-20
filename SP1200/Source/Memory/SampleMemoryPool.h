#pragma once

#include "../SP1200Constants.h"
#include "TwelveBitBuffer.h"

#include <optional>
#include <string>
#include <vector>

namespace sp1200
{
struct SampleSegment
{
    std::string name;
    int bank = 0; // 0..3 → A..D
    TwelveBitBuffer data;
};

class SampleMemoryPool
{
public:
    [[nodiscard]] std::int64_t maxSamples() const noexcept { return kMaxMemorySamples; }
    [[nodiscard]] std::int64_t usedSamples() const noexcept { return usedSamples_; }
    [[nodiscard]] std::int64_t freeSamples() const noexcept { return maxSamples() - usedSamples_; }

    /** Returns segment index or nullopt if over 7:00 cap. */
    std::optional<std::size_t> appendSegment (SampleSegment segment);

    [[nodiscard]] const SampleSegment* getSegment (std::size_t index) const;
    [[nodiscard]] SampleSegment* getSegment (std::size_t index);

    void clear();

private:
    void recomputeUsed();

    std::vector<SampleSegment> segments_;
    std::int64_t usedSamples_ = 0;
};

} // namespace sp1200
