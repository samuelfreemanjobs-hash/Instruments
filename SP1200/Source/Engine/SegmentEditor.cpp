#include "SegmentEditor.h"

#include <algorithm>
#include <cmath>

namespace sp1200
{
std::vector<float> segmentToFloat (const SampleSegment& seg)
{
    std::vector<float> out (seg.data.size());
    seg.data.writeToFloat (out.data(), out.size());
    return out;
}

bool truncateSegmentInPlace (SampleMemoryPool& pool, std::size_t index, std::int64_t start, std::int64_t end)
{
    auto* seg = pool.getSegment (index);
    if (seg == nullptr)
        return false;

    const auto n = static_cast<std::int64_t> (seg->data.size());
    start = std::clamp (start, std::int64_t { 0 }, n);
    end = std::clamp (end, start, n);
    if (end <= start)
        return false;

    const auto len = static_cast<std::size_t> (end - start);
    TwelveBitBuffer trimmed;
    trimmed.resize (len);
    for (std::size_t i = 0; i < len; ++i)
        trimmed.setSample (i, seg->data.getSample (static_cast<std::size_t> (start) + i));

    seg->data = std::move (trimmed);
    pool.recomputeUsedPublic();
    return true;
}

bool reverseSegmentInPlace (SampleMemoryPool& pool, std::size_t index)
{
    auto* seg = pool.getSegment (index);
    if (seg == nullptr || seg->data.empty())
        return false;

    const auto n = seg->data.size();
    for (std::size_t i = 0; i < n / 2; ++i)
    {
        const auto a = seg->data.getSample (i);
        const auto b = seg->data.getSample (n - 1 - i);
        seg->data.setSample (i, b);
        seg->data.setSample (n - 1 - i, a);
    }
    return true;
}

bool normalizeSegmentInPlace (SampleMemoryPool& pool, std::size_t index)
{
    auto* seg = pool.getSegment (index);
    if (seg == nullptr || seg->data.empty())
        return false;

    auto floats = segmentToFloat (*seg);
    float peak = 0.0f;
    for (float s : floats)
        peak = std::max (peak, std::abs (s));
    if (peak < 1.0e-6f)
        return false;

    const float gain = 0.95f / peak;
    for (std::size_t i = 0; i < floats.size(); ++i)
        floats[i] *= gain;
    seg->data.quantizeFromFloat (floats.data(), floats.size());
    return true;
}

void remapPadSegmentIndex (PadBank& pads, std::size_t removedIndex, int replacementIndex)
{
    const int removed = static_cast<int> (removedIndex);
    for (int p = 0; p < kNumPads; ++p)
    {
        int& idx = pads.pads[p].segmentIndex;
        if (idx == removed)
            idx = replacementIndex;
        else if (idx > removed)
            --idx;
    }
}

bool setSegmentBank (SampleMemoryPool& pool, std::size_t index, int bankIndex)
{
    if (bankIndex < 0 || bankIndex >= kNumBanks)
        return false;

    auto* seg = pool.getSegment (index);
    if (seg == nullptr)
        return false;

    seg->bank = bankIndex;
    return true;
}

bool combineSegments (SampleMemoryPool& pool, PadBank& pads, std::size_t indexA, std::size_t indexB)
{
    if (indexA == indexB)
        return false;

    auto* segA = pool.getSegment (indexA);
    auto* segB = pool.getSegment (indexB);
    if (segA == nullptr || segB == nullptr || segB->data.empty())
        return false;

    const std::size_t primary = indexA;
    const std::size_t secondary = indexB;
    auto* primarySeg = pool.getSegment (primary);
    const std::size_t offset = primarySeg->data.size();
    const std::size_t appendLen = segB->data.size();
    primarySeg->data.resize (offset + appendLen);
    for (std::size_t i = 0; i < appendLen; ++i)
        primarySeg->data.setSample (offset + i, segB->data.getSample (i));

    const int replacementIndex =
        static_cast<int> (primary > secondary ? primary - 1 : primary);

    pool.removeSegment (secondary);
    pool.recomputeUsedPublic();
    remapPadSegmentIndex (pads, secondary, replacementIndex);
    return true;
}

} // namespace sp1200
