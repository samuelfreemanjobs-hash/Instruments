#pragma once

#include <juce_core/juce_core.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace junovax::dsp
{
/** FFT-built mip-map bandlimited saw (PWM = saw phase diff, sub = half-rate square). */
class MipBlepOscillators
{
public:
    static constexpr int kTableSize = 2048;
    static constexpr int kNumMips = 12;

    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
        rebuildMipTables();
    }

    float saw (float phase01, float hz) const noexcept
    {
        const int mip = mipIndexForHz (hz);
        return lookup (mip, phase01);
    }

    float pulsePwm (float phase01, float pwm01, float hz) const noexcept
    {
        pwm01 = std::clamp (pwm01, 0.01f, 0.99f);
        return saw (phase01, hz) - saw (wrap01 (phase01 + pwm01), hz);
    }

    float subSquare (float subPhase01, float hz, float divisor) const noexcept
    {
        divisor = std::max (0.25f, divisor);
        const float subHz = hz / divisor;
        return saw (subPhase01, subHz) - saw (wrap01 (subPhase01 + 0.5f), subHz);
    }

    float naiveSaw (float phase01) const noexcept
    {
        return 2.0f * phase01 - 1.0f;
    }

private:
    static float wrap01 (float p) noexcept
    {
        p -= std::floor (p);
        if (p < 0.0f)
            p += 1.0f;
        return p;
    }

    int mipIndexForHz (float hz) const noexcept
    {
        hz = std::max (1.0f, hz);
        const int maxHarmonic = static_cast<int> (std::floor (0.495 * sampleRate_ / hz));
        if (maxHarmonic < 1)
            return kNumMips - 1;
        for (int mip = 0; mip < kNumMips; ++mip)
        {
            const int limit = maxHarmonicsForMip (mip);
            if (maxHarmonic <= limit)
                return mip;
        }
        return 0;
    }

    int maxHarmonicsForMip (int mip) const noexcept
    {
        return 1 << (kNumMips - 1 - mip);
    }

    void rebuildMipTables() noexcept
    {
        for (int mip = 0; mip < kNumMips; ++mip)
            buildMip (mip, maxHarmonicsForMip (mip));
    }

    void buildMip (int mip, int maxHarmonic) noexcept
    {
        std::vector<float> time (static_cast<std::size_t> (kTableSize), 0.0f);
        for (int n = 0; n < kTableSize; ++n)
        {
            const float t = static_cast<float> (n) / static_cast<float> (kTableSize);
            float sum = 0.0f;
            for (int h = 1; h <= maxHarmonic; ++h)
            {
                float taper = 1.0f;
                if (maxHarmonic >= 4 && h > maxHarmonic - 3)
                {
                    const float d = static_cast<float> (maxHarmonic - h + 1) / 3.0f;
                    taper = d * d * (3.0f - 2.0f * d);
                }
                const float amp = taper * -2.0f / (juce::MathConstants<float>::pi * static_cast<float> (h));
                sum += amp * std::sin (juce::MathConstants<float>::twoPi * static_cast<float> (h) * t);
            }
            time[static_cast<std::size_t> (n)] = sum;
        }
        float peak = 0.0f;
        for (float s : time)
            peak = std::max (peak, std::abs (s));
        if (peak > 1.0e-6f)
        {
            for (float& s : time)
                s /= peak;
        }
        auto& table = mipTables_[static_cast<std::size_t> (mip)];
        for (int i = 0; i < kTableSize; ++i)
            table[static_cast<std::size_t> (i)] = time[static_cast<std::size_t> (i)];
        table[0] = table[static_cast<std::size_t> (kTableSize - 1)];
    }

    float lookup (int mip, float phase01) const noexcept
    {
        mip = std::clamp (mip, 0, kNumMips - 1);
        const auto& table = mipTables_[static_cast<std::size_t> (mip)];
        const float pos = wrap01 (phase01) * static_cast<float> (kTableSize);
        const int i0 = static_cast<int> (pos) % kTableSize;
        const int i1 = (i0 + 1) % kTableSize;
        const float frac = pos - std::floor (pos);
        return table[static_cast<std::size_t> (i0)] * (1.0f - frac) + table[static_cast<std::size_t> (i1)] * frac;
    }

    double sampleRate_ = 44100.0;
    std::array<std::array<float, kTableSize>, kNumMips> mipTables_{};
};
} // namespace junovax::dsp