#pragma once

#include <algorithm>
#include <cmath>

namespace wave909::dsp
{

class DestructiveStage final
{
public:
    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
    }

    void reset() noexcept
    {
        holdCounter_ = 0;
        heldSample_ = 0.0f;
    }

    float process (float input, float driveNorm, float crushNorm, bool destructive) noexcept
    {
        if (! destructive)
            return std::tanh (input * (1.0f + driveNorm * 0.5f));

        const float driven = std::tanh (input * (1.0f + driveNorm * 8.0f));
        const float crushed = quantize (driven, crushNorm);

        const int holdSamples = 1 + static_cast<int> (crushNorm * 24.0f);
        if (++holdCounter_ >= holdSamples)
        {
            holdCounter_ = 0;
            heldSample_ = crushed;
        }

        return heldSample_;
    }

private:
    static float quantize (float x, float crushNorm) noexcept
    {
        crushNorm = std::clamp (crushNorm, 0.0f, 1.0f);
        const float steps = 2.0f + (1.0f - crushNorm) * 254.0f;
        return std::round (x * steps) / steps;
    }

    double sampleRate_ = 44100.0;
    int holdCounter_ = 0;
    float heldSample_ = 0.0f;
};

} // namespace wave909::dsp
