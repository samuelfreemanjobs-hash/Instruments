#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>

namespace aftrdark::dsp
{

constexpr std::size_t kWavetableCount = 64;
constexpr std::size_t kWavetableSize = 2048;

/** Procedural 64-wave bank for trap/phonk morphs (sampleless). */
class WavetableBank
{
public:
    void rebuild() noexcept
    {
        for (std::size_t w = 0; w < kWavetableCount; ++w)
            buildWave (w, tables_[w]);
    }

    const std::array<float, kWavetableSize>& getTable (std::size_t index) const noexcept
    {
        const auto idx = std::min (index, kWavetableCount - 1);
        return tables_[idx];
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

    float sampleMorph (double phase01, float positionNorm) const noexcept
    {
        positionNorm = std::clamp (positionNorm, 0.0f, 1.0f);
        const float scaled = positionNorm * static_cast<float> (kWavetableCount - 1);
        const auto i0 = static_cast<std::size_t> (scaled);
        const auto i1 = std::min (i0 + 1, kWavetableCount - 1);
        const float frac = scaled - static_cast<float> (i0);

        const float a = sampleAt (tables_[i0], phase01);
        const float b = sampleAt (tables_[i1], phase01);
        return a + (b - a) * frac;
    }

private:
    static void buildWave (std::size_t waveIndex, std::array<float, kWavetableSize>& out) noexcept
    {
        const float t = static_cast<float> (waveIndex) / static_cast<float> (kWavetableCount - 1);
        const int maxHarm = static_cast<int> (2.0f + t * 46.0f);

        for (std::size_t i = 0; i < kWavetableSize; ++i)
        {
            const double phase = static_cast<double> (i) / static_cast<double> (kWavetableSize);
            double sum = 0.0;
            for (int h = 1; h <= maxHarm; ++h)
            {
                const double norm = 1.0 / static_cast<double> (h * h);
                const double phaseShift = static_cast<double> (waveIndex) * 0.11 * static_cast<double> (h);
                sum += norm * std::sin (juceTwoPi * phase * static_cast<double> (h) + phaseShift);
            }

            const double fm = std::sin (juceTwoPi * phase * (3.0 + t * 7.0)) * (0.08 + t * 0.25);
            const double grit = (waveIndex % 7 == 0) ? std::sin (juceTwoPi * phase * 13.0) * 0.15 : 0.0;
            out[i] = static_cast<float> (sum * (0.35 + t * 0.15) + fm + grit);
        }

        normalize (out);
    }

    static void normalize (std::array<float, kWavetableSize>& table) noexcept
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

    static constexpr double juceTwoPi = 6.283185307179586;

    std::array<std::array<float, kWavetableSize>, kWavetableCount> tables_{};
};

} // namespace aftrdark::dsp
