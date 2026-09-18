#pragma once

#include <algorithm>
#include <cmath>

namespace aftrdark::dsp
{

class ZdfLowpass final
{
public:
    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
    }

    void reset() noexcept
    {
        ic1eq_ = 0.0f;
        ic2eq_ = 0.0f;
    }

    float process (float input, float cutoffNorm, float resonanceNorm) noexcept
    {
        cutoffNorm = std::clamp (cutoffNorm, 0.0f, 1.0f);
        resonanceNorm = std::clamp (resonanceNorm, 0.0f, 1.0f);

        const float fcHz = 20.0f + std::pow (cutoffNorm, 2.2f) * 19800.0f;
        const float g = std::tan (3.14159265f * fcHz / static_cast<float> (sampleRate_));
        const float k = 0.25f + resonanceNorm * 2.75f;

        const float limitedFeedback = std::tanh (ic2eq_ * (1.0f + resonanceNorm * 5.0f));
        const float x = input - limitedFeedback * (resonanceNorm * 1.25f);

        const float a1 = 1.0f / (1.0f + g * (g + k));
        const float a2 = g * a1;
        const float a3 = g * a2;

        const float v3 = x - ic2eq_;
        const float v1 = a1 * ic1eq_ + a2 * v3;
        const float v2 = ic2eq_ + a2 * ic1eq_ + a3 * v3;

        ic1eq_ = 2.0f * v1 - ic1eq_;
        ic2eq_ = 2.0f * v2 - ic2eq_;

        return v2;
    }

private:
    double sampleRate_ = 44100.0;
    float ic1eq_ = 0.0f;
    float ic2eq_ = 0.0f;
};

} // namespace aftrdark::dsp
