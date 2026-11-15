#pragma once

#include "WavetableFft.h"

#include <juce_core/juce_core.h>

#include <algorithm>
#include <array>
#include <cmath>

namespace junovax::dsp
{
/** WAVE-9090-style per-octave FFT mip saw with Hermite lookup and mip crossfade. */
class OctaveMipSawBank
{
public:
    static constexpr std::size_t kTableSize = 2048;
    static constexpr std::size_t kMipLevels = 11;
    static constexpr std::size_t kHarmSafetyBins = 2;

    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
        rebuild();
    }

    float saw (double phase01, double phaseInc) const noexcept
    {
        const MipSlice mip = resolveMip (phaseInc);
        const float a = sampleAt (mipTables_[static_cast<std::size_t> (mip.low)], phase01);
        if (mip.high == mip.low)
            return a;
        const float b = sampleAt (mipTables_[static_cast<std::size_t> (mip.high)], phase01);
        return a + (b - a) * mip.xf;
    }

    float pulsePwm (double phase01, float pwm01, double phaseInc) const noexcept
    {
        pwm01 = std::clamp (pwm01, 0.01f, 0.99f);
        double p2 = phase01 + static_cast<double> (pwm01);
        p2 -= std::floor (p2);
        return saw (phase01, phaseInc) - saw (p2, phaseInc);
    }

    float subSquare (double subPhase01, double subPhaseInc) const noexcept
    {
        double p2 = subPhase01 + 0.5;
        p2 -= std::floor (p2);
        return saw (subPhase01, subPhaseInc) - saw (p2, subPhaseInc);
    }

    float naiveSaw (float phase01) const noexcept
    {
        return 2.0f * phase01 - 1.0f;
    }

private:
    struct MipSlice
    {
        int low = 0;
        int high = 0;
        float xf = 0.0f;
    };

    static void removeDc (std::array<float, kTableSize>& table) noexcept
    {
        double mean = 0.0;
        for (float s : table)
            mean += static_cast<double> (s);
        mean /= static_cast<double> (kTableSize);
        for (float& s : table)
            s -= static_cast<float> (mean);
    }

    static void normalize (std::array<float, kTableSize>& table) noexcept
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

    static void buildIdealSaw (std::array<float, kTableSize>& out) noexcept
    {
        constexpr double twoPi = 6.283185307179586;
        for (std::size_t i = 0; i < kTableSize; ++i)
        {
            const double phase = static_cast<double> (i) / static_cast<double> (kTableSize);
            double sum = 0.0;
            const int maxH = static_cast<int> (kTableSize / 2) - 1;
            for (int h = 1; h <= maxH; ++h)
                sum += (-2.0 / (3.141592653589793 * static_cast<double> (h))) * std::sin (twoPi * phase * static_cast<double> (h));
            out[i] = static_cast<float> (sum);
        }
    }

    void rebuild() noexcept
    {
        std::array<float, kTableSize> full{};
        buildIdealSaw (full);
        removeDc (full);
        normalize (full);

        for (std::size_t mip = 0; mip < kMipLevels; ++mip)
        {
            const int maxHarm = std::max (1, static_cast<int> ((kTableSize / 2) >> mip) - static_cast<int> (kHarmSafetyBins));
            bandLimitCycle (full, maxHarm, mipTables_[mip]);
            removeDc (mipTables_[mip]);
            normalize (mipTables_[mip]);
        }
    }

    MipSlice resolveMip (double phaseInc) const noexcept
    {
        phaseInc = std::clamp (phaseInc, 1.0e-9, 0.49);
        constexpr double kSafety = 0.42;
        const double mipFloat = std::log2 ((phaseInc * static_cast<double> (kTableSize)) / kSafety);
        const float clamped = static_cast<float> (std::clamp (mipFloat, 0.0, static_cast<double> (kMipLevels - 1)));
        MipSlice slice;
        slice.low = static_cast<int> (std::floor (clamped));
        slice.high = std::min (slice.low + 1, static_cast<int> (kMipLevels) - 1);
        slice.xf = clamped - static_cast<float> (slice.low);
        return slice;
    }

    static float sampleAt (const std::array<float, kTableSize>& table, double phase01) noexcept
    {
        phase01 -= std::floor (phase01);
        const double pos = phase01 * static_cast<double> (kTableSize);
        const auto i0 = static_cast<std::size_t> (pos) % kTableSize;
        const auto i1 = (i0 + 1) % kTableSize;
        const float frac = static_cast<float> (pos - std::floor (pos));

        const float y0 = table[i0];
        const float y1 = table[i1];
        const float ym1 = table[(i0 + kTableSize - 1) % kTableSize];
        const float y2 = table[(i1 + 1) % kTableSize];

        const float c0 = y0;
        const float c1 = 0.5f * (y1 - ym1);
        const float c2 = ym1 - 2.5f * y0 + 2.0f * y1 - 0.5f * y2;
        const float c3 = 0.5f * (y2 - ym1) + 1.5f * (y0 - y1);
        return ((c3 * frac + c2) * frac + c1) * frac + c0;
    }

    double sampleRate_ = 44100.0;
    std::array<std::array<float, kTableSize>, kMipLevels> mipTables_{};
};
} // namespace junovax::dsp