#include "DSP/WavetableBank.h"
#include "DSP/WavetableFft.h"

#include <algorithm>
#include <cmath>

namespace wave9090::dsp
{

void WavetableBank::rebuild() noexcept
{
    for (std::size_t w = 0; w < kWavetableCount; ++w)
    {
        std::array<float, kWavetableSize> full{};
        buildHarmonicCycle (w, harmonicsForWave (w), full);
        removeDc (full);
        normalize (full);
        legacyTables_[w] = full;

        for (std::size_t mip = 0; mip < kMipLevels; ++mip)
        {
            const int maxHarm = std::max (1, static_cast<int> ((kWavetableSize / 2) >> mip) - static_cast<int> (kHarmSafetyBins));
            bandLimitCycle (full, maxHarm, mipTables_[mip][w]);
            removeDc (mipTables_[mip][w]);
            normalize (mipTables_[mip][w]);
        }
    }
}

void WavetableBank::buildHarmonicCycle (std::size_t waveIndex, int maxHarm, std::array<float, kWavetableSize>& out) noexcept
{
    const float t = static_cast<float> (waveIndex) / static_cast<float> (kWavetableCount - 1);

    for (std::size_t i = 0; i < kWavetableSize; ++i)
    {
        const double phase = static_cast<double> (i) / static_cast<double> (kWavetableSize);
        double sum = 0.0;
        const int harmLimit = std::min (maxHarm, harmonicsForWave (waveIndex));
        for (int h = 1; h <= harmLimit; ++h)
        {
            const double norm = 1.0 / static_cast<double> (h * h);
            const double phaseShift = static_cast<double> (waveIndex) * 0.11 * static_cast<double> (h);
            sum += norm * std::sin (WavetableBank::juceTwoPi * phase * static_cast<double> (h) + phaseShift);
        }
        out[i] = static_cast<float> (sum * (0.35 + t * 0.15));
    }
}

float WavetableBank::mipBlend (double phaseInc) const noexcept
{
    phaseInc = std::clamp (phaseInc, 1.0e-9, 0.49);
    constexpr double kSafety = 0.42;
    const double mipFloat = std::log2 ((phaseInc * static_cast<double> (kWavetableSize)) / kSafety);
    return static_cast<float> (std::clamp (mipFloat, 0.0, static_cast<double> (kMipLevels - 1)));
}

float WavetableBank::sampleMorph (double phase01, float positionNorm, double phaseInc) const noexcept
{
    positionNorm = std::clamp (positionNorm, 0.0f, 1.0f);
    const float scaled = positionNorm * static_cast<float> (kWavetableCount - 1);
    const auto i0 = static_cast<std::size_t> (scaled);
    const auto i1 = std::min (i0 + 1, kWavetableCount - 1);
    const float frac = scaled - static_cast<float> (i0);

    const float mipF = mipBlend (phaseInc);
    const int mipLow = static_cast<int> (std::floor (mipF));
    const int mipHigh = std::min (mipLow + 1, static_cast<int> (kMipLevels) - 1);
    const float mipXf = mipF - static_cast<float> (mipLow);

    const float a = sampleAt (mipTables_[static_cast<std::size_t> (mipLow)][i0], phase01);
    const float b = sampleAt (mipTables_[static_cast<std::size_t> (mipLow)][i1], phase01);
    const float c = sampleAt (mipTables_[static_cast<std::size_t> (mipHigh)][i0], phase01);
    const float d = sampleAt (mipTables_[static_cast<std::size_t> (mipHigh)][i1], phase01);

    const float morphLow = a + (b - a) * frac;
    if (mipHigh == mipLow)
        return morphLow;
    const float morphHigh = c + (d - c) * frac;
    return morphLow + (morphHigh - morphLow) * mipXf;
}

float WavetableBank::sampleLegacyMorph (double phase01, float positionNorm, double phaseInc) const noexcept
{
    (void) phaseInc;
    positionNorm = std::clamp (positionNorm, 0.0f, 1.0f);
    const float scaled = positionNorm * static_cast<float> (kWavetableCount - 1);
    const auto i0 = static_cast<std::size_t> (scaled);
    const auto i1 = std::min (i0 + 1, kWavetableCount - 1);
    const float frac = scaled - static_cast<float> (i0);
    const float a = sampleAt (legacyTables_[i0], phase01);
    const float b = sampleAt (legacyTables_[i1], phase01);
    return a + (b - a) * frac;
}

float WavetableBank::sampleSine (double phase01, double phaseInc) const noexcept
{
    const float mipF = mipBlend (phaseInc);
    const int mipLow = static_cast<int> (std::floor (mipF));
    const int mipHigh = std::min (mipLow + 1, static_cast<int> (kMipLevels) - 1);
    const float mipXf = mipF - static_cast<float> (mipLow);
    const float a = sampleAt (mipTables_[static_cast<std::size_t> (mipLow)][0], phase01);
    const float b = sampleAt (mipTables_[static_cast<std::size_t> (mipHigh)][0], phase01);
    return a + (b - a) * mipXf;
}

void WavetableBank::removeDc (std::array<float, kWavetableSize>& table) noexcept
{
    double mean = 0.0;
    for (float s : table)
        mean += static_cast<double> (s);
    mean /= static_cast<double> (kWavetableSize);
    for (float& s : table)
        s -= static_cast<float> (mean);
}

void WavetableBank::normalize (std::array<float, kWavetableSize>& table) noexcept
{
    float peak = 0.0f;
    for (float s : table)
        peak = std::max (peak, std::abs (s));
    if (peak > 1.0e-6f)
    {
        const float g = 0.95f / peak;
        for (float& s : table)
            s *= g;
    }
}

int WavetableBank::harmonicsForWave (std::size_t waveIndex) noexcept
{
    const float t = static_cast<float> (waveIndex) / static_cast<float> (kWavetableCount - 1);
    return static_cast<int> (2.0f + t * 46.0f);
}

} // namespace wave9090::dsp