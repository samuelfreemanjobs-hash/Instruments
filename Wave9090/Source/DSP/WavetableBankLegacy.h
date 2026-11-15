#pragma once

#include "DSP/WavetableBank.h"

namespace wave9090::dsp
{

/** Pre–elite-pass mip selection (for aliasing A/B measurements). */
inline std::size_t legacySelectMip (double phaseInc) noexcept
{
    const double cycles = phaseInc * static_cast<double> (kWavetableSize);
    if (cycles < 2.0)
        return 0;
    const int mip = static_cast<int> (std::floor (std::log2 (cycles)));
    return static_cast<std::size_t> (std::clamp (mip, 0, static_cast<int> (kMipLevels) - 1));
}

inline float legacySampleMorph (const WavetableBank& bank, double phase01, float positionNorm, double phaseInc) noexcept
{
    positionNorm = std::clamp (positionNorm, 0.0f, 1.0f);
    const float scaled = positionNorm * static_cast<float> (kWavetableCount - 1);
    const auto i0 = static_cast<std::size_t> (scaled);
    const auto i1 = std::min (i0 + 1, kWavetableCount - 1);
    const float frac = scaled - static_cast<float> (i0);
    const auto mip = legacySelectMip (phaseInc);
    const float a = WavetableBank::sampleAt (bank.getTable (i0, mip), phase01);
    const float b = WavetableBank::sampleAt (bank.getTable (i1, mip), phase01);
    return a + (b - a) * frac;
}

} // namespace wave9090::dsp