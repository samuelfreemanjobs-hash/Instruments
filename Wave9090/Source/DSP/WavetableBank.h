#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>

namespace wave9090::dsp
{

constexpr std::size_t kWavetableCount = 64;
constexpr std::size_t kWavetableSize = 2048;
constexpr std::size_t kMipLevels = 11;
constexpr std::size_t kHarmSafetyBins = 2;

/** Cached frame + mip indices for block-rate wavetable morph (no per-sample log2). */
struct MorphContext
{
    std::size_t waveIndex0 = 0;
    std::size_t waveIndex1 = 0;
    float morphFrac = 0.0f;
    int mipLow = 0;
    int mipHigh = 0;
    float mipXf = 0.0f;
};

/** 64-wave bank with FFT band-limited per-octave mip tables. */
class WavetableBank
{
public:
    void rebuild() noexcept;

    MorphContext buildMorphContext (float positionNorm, double phaseInc) const noexcept;
    float sampleMorphContext (double phase01, const MorphContext& ctx) const noexcept;
    float sampleSineContext (double phase01, const MorphContext& ctx) const noexcept;

    const std::array<float, kWavetableSize>& getTable (std::size_t index, std::size_t mip = 0) const noexcept
    {
        const auto idx = std::min (index, kWavetableCount - 1);
        const auto lvl = std::min (mip, kMipLevels - 1);
        return mipTables_[lvl][idx];
    }

    const std::array<float, kWavetableSize>& getLegacyTable (std::size_t index) const noexcept
    {
        return legacyTables_[std::min (index, kWavetableCount - 1)];
    }

    static float sampleAt (const std::array<float, kWavetableSize>& table, double phase01) noexcept
    {
        phase01 -= std::floor (phase01);
        const double pos = phase01 * static_cast<double> (kWavetableSize);
        const auto i0 = static_cast<std::size_t> (pos) % kWavetableSize;
        const auto i1 = (i0 + 1) % kWavetableSize;
        const float frac = static_cast<float> (pos - std::floor (pos));

        const float y0 = table[i0];
        const float y1 = table[i1];
        const float ym1 = table[(i0 + kWavetableSize - 1) % kWavetableSize];
        const float y2 = table[(i1 + 1) % kWavetableSize];

        const float c0 = y0;
        const float c1 = 0.5f * (y1 - ym1);
        const float c2 = ym1 - 2.5f * y0 + 2.0f * y1 - 0.5f * y2;
        const float c3 = 0.5f * (y2 - ym1) + 1.5f * (y0 - y1);
        return ((c3 * frac + c2) * frac + c1) * frac + c0;
    }

    /** Continuous mip index 0..kMipLevels-1 from cycles/sample (includes safety margin). */
    float mipBlend (double phaseInc) const noexcept;

    float sampleMorph (double phase01, float positionNorm, double phaseInc) const noexcept;

    /** Unband-limited morph (full harmonic table) for A/B measurements. */
    float sampleLegacyMorph (double phase01, float positionNorm, double phaseInc) const noexcept;

    float sampleSine (double phase01, double phaseInc) const noexcept;

private:
    static void buildHarmonicCycle (std::size_t waveIndex, int maxHarm, std::array<float, kWavetableSize>& out) noexcept;
    static int harmonicsForWave (std::size_t waveIndex) noexcept;
    static void removeDc (std::array<float, kWavetableSize>& table) noexcept;
    static void normalize (std::array<float, kWavetableSize>& table) noexcept;

    static constexpr double juceTwoPi = 6.283185307179586;

    std::array<std::array<std::array<float, kWavetableSize>, kWavetableCount>, kMipLevels> mipTables_{};
    std::array<std::array<float, kWavetableSize>, kWavetableCount> legacyTables_{};
};

} // namespace wave9090::dsp