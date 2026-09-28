#include "TransientChop.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace sp1200
{
namespace
{
struct Onset
{
    std::int64_t index;
    float strength;
};

float sampleAt (const float* mono, std::size_t numSamples, std::int64_t i)
{
    if (i < 0 || static_cast<std::size_t> (i) >= numSamples)
        return 0.0f;
    return mono[static_cast<std::size_t> (i)];
}
} // namespace

std::vector<SliceBoundary> detectTransientSlices (const float* mono,
                                                  std::size_t numSamples,
                                                  std::int64_t regionStart,
                                                  std::int64_t regionEnd,
                                                  int numSlices,
                                                  double sampleRateHz,
                                                  double minGapSeconds)
{
    std::vector<SliceBoundary> out;
    if (mono == nullptr || numSamples == 0 || numSlices <= 0)
        return out;

    regionStart = std::max (std::int64_t { 0 }, regionStart);
    regionEnd = std::min (static_cast<std::int64_t> (numSamples), regionEnd);
    if (regionEnd - regionStart < static_cast<std::int64_t> (sampleRateHz * 0.05))
        return out;

    const int window = static_cast<int> (sampleRateHz * 0.023); // ~23 ms
    const int hop = std::max (1, window / 4);
    const auto minGapSamples = static_cast<std::int64_t> (minGapSeconds * sampleRateHz);

    std::vector<Onset> onsets;
    float prevEnergy = 0.0f;

    for (std::int64_t i = regionStart; i + window < regionEnd; i += hop)
    {
        float energy = 0.0f;
        for (int w = 0; w < window; ++w)
        {
            const float s = sampleAt (mono, numSamples, i + w);
            energy += s * s;
        }
        energy /= static_cast<float> (window);
        const float flux = std::max (0.0f, energy - prevEnergy);
        prevEnergy = energy;
        if (flux > 0.0005f)
            onsets.push_back ({ i, flux });
    }

    std::sort (onsets.begin(), onsets.end(), [] (const Onset& a, const Onset& b)
               {
                   return a.strength > b.strength;
               });

    std::vector<std::int64_t> cutPoints;
    cutPoints.push_back (regionStart);
    cutPoints.push_back (regionEnd);

    for (const auto& o : onsets)
    {
        if (static_cast<int> (cutPoints.size()) - 2 >= numSlices - 1)
            break;
        bool tooClose = false;
        for (const auto c : cutPoints)
        {
            if (std::abs (c - o.index) < minGapSamples)
            {
                tooClose = true;
                break;
            }
        }
        if (! tooClose)
            cutPoints.push_back (o.index);
    }

    std::sort (cutPoints.begin(), cutPoints.end());
    cutPoints.erase (std::unique (cutPoints.begin(), cutPoints.end()), cutPoints.end());

    while (static_cast<int> (cutPoints.size()) - 1 < numSlices && cutPoints.size() >= 2)
    {
        std::int64_t bestLen = 0;
        std::size_t bestIdx = 0;
        for (std::size_t i = 0; i + 1 < cutPoints.size(); ++i)
        {
            const auto len = cutPoints[i + 1] - cutPoints[i];
            if (len > bestLen)
            {
                bestLen = len;
                bestIdx = i;
            }
        }
        if (bestLen < minGapSamples * 2)
            break;
        cutPoints.insert (cutPoints.begin() + static_cast<std::ptrdiff_t> (bestIdx + 1),
                          cutPoints[bestIdx] + bestLen / 2);
    }

    if (static_cast<int> (cutPoints.size()) - 1 > numSlices)
    {
        while (static_cast<int> (cutPoints.size()) - 1 > numSlices)
        {
            std::int64_t minLen = std::numeric_limits<std::int64_t>::max();
            std::size_t minIdx = 1;
            for (std::size_t i = 1; i + 1 < cutPoints.size(); ++i)
            {
                const auto len = cutPoints[i + 1] - cutPoints[i];
                if (len < minLen)
                {
                    minLen = len;
                    minIdx = i;
                }
            }
            cutPoints.erase (cutPoints.begin() + static_cast<std::ptrdiff_t> (minIdx));
        }
    }

    for (std::size_t i = 0; i + 1 < cutPoints.size(); ++i)
        out.push_back ({ cutPoints[i], cutPoints[i + 1] });

    return out;
}

} // namespace sp1200
