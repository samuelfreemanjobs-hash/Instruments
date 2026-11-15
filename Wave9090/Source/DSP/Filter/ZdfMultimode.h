#pragma once

#include "DSP/DspUtils.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <tuple>

namespace wave9090::dsp
{

/** Trapezoidal SVF — LP / BP / HP at 12 or 24 dB (cascade). */
class ZdfMultimode final
{
public:
    enum class Mode : std::uint8_t
    {
        lowPass12 = 0,
        lowPass24,
        bandPass12,
        bandPass24,
        highPass12,
        highPass24,
    };

    void prepare (double sampleRate) noexcept { sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0; }

    void reset() noexcept
    {
        ic1eq_ = ic2eq_ = 0.0f;
        ic1eqB_ = ic2eqB_ = 0.0f;
    }

    void setMode (Mode m) noexcept { mode_ = m; }

    float process (float input, float cutoffNorm, float resonanceNorm) noexcept
    {
        cutoffNorm = std::clamp (cutoffNorm, 0.0f, 1.0f);
        resonanceNorm = std::clamp (resonanceNorm, 0.0f, 1.0f);

        const float fcHz = 20.0f + std::pow (cutoffNorm, 2.2f) * 19800.0f;
        const float g = std::tan (3.14159265f * fcHz / static_cast<float> (sampleRate_));
        const float k = 0.5f + resonanceNorm * 1.75f;
        const float R = 1.0f / std::max (0.01f, k);

        const auto stage = [&] (float x, float& ic1eq, float& ic2eq) -> std::tuple<float, float, float>
        {
            const float v3 = x - ic2eq;
            const float a1 = 1.0f / (1.0f + g * (g + R));
            const float a2 = g * a1;
            const float a3 = g * a2;
            const float v1 = a1 * ic1eq + a2 * v3;
            const float v2 = ic2eq + a2 * ic1eq + a3 * v3;
            ic1eq = 2.0f * v1 - ic1eq;
            ic2eq = 2.0f * v2 - ic2eq;
            const float lp = v2;
            const float bp = g * v1;
            const float hp = x - k * bp - lp;
            return { lp, bp, hp };
        };

        const float x = flushDenormal (input);
        auto [lp1, bp1, hp1] = stage (x, ic1eq_, ic2eq_);

        const bool use24 = (mode_ == Mode::lowPass24 || mode_ == Mode::bandPass24 || mode_ == Mode::highPass24);
        float lp = lp1;
        float bp = bp1;
        float hp = hp1;
        if (use24)
        {
            auto [lp2, bp2, hp2] = stage (lp1, ic1eqB_, ic2eqB_);
            lp = lp2;
            bp = 0.5f * (bp1 + bp2);
            hp = hp2;
        }

        switch (mode_)
        {
            case Mode::lowPass12:
            case Mode::lowPass24:
                return lp;
            case Mode::bandPass12:
            case Mode::bandPass24:
                return bp;
            case Mode::highPass12:
            case Mode::highPass24:
                return hp;
        }
        return lp;
    }

private:
    double sampleRate_ = 44100.0;
    Mode mode_ = Mode::lowPass24;
    float ic1eq_ = 0.0f;
    float ic2eq_ = 0.0f;
    float ic1eqB_ = 0.0f;
    float ic2eqB_ = 0.0f;
};

} // namespace wave9090::dsp