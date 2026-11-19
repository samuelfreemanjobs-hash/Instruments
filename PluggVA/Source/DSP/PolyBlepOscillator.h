#pragma once

#include "Engine/SynthParams.h"

#include <algorithm>
#include <cmath>

namespace pluggva::dsp
{

class PolyBlepOscillator final
{
public:
    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
    }

    void reset() noexcept
    {
        phase_ = 0.0;
        integrator_ = 0.0;
    }

    void setFrequency (float hz) noexcept
    {
        hz = std::clamp (hz, 0.0f, static_cast<float> (sampleRate_ * 0.45));
        phaseInc_ = hz / static_cast<float> (sampleRate_);
    }

    float process (Waveform wave, float pulseWidth) noexcept
    {
        pulseWidth = std::clamp (pulseWidth, 0.05f, 0.95f);
        const float t = static_cast<float> (phase_);
        const float dt = phaseInc_;

        float out = 0.0f;
        switch (wave)
        {
            case Waveform::saw:
                out = 2.0f * t - 1.0f - polyBlep (t, dt);
                break;
            case Waveform::square:
            {
                out = t < 0.5f ? 1.0f : -1.0f;
                out += polyBlep (t, dt);
                out -= polyBlep (std::fmod (t + 0.5f, 1.0f), dt);
                break;
            }
            case Waveform::pulse:
            {
                out = t < pulseWidth ? 1.0f : -1.0f;
                out += polyBlep (t, dt);
                const float t2 = std::fmod (t + (1.0f - pulseWidth), 1.0f);
                out -= polyBlep (t2, dt);
                break;
            }
            case Waveform::triangle:
            {
                const float sq = t < 0.5f ? 1.0f : -1.0f;
                const float blep = polyBlep (t, dt) - polyBlep (std::fmod (t + 0.5f, 1.0f), dt);
                integrator_ += (sq + blep) * dt * 4.0f;
                integrator_ = std::clamp (integrator_, -1.0f, 1.0f);
                out = integrator_;
                break;
            }
        }

        phase_ += phaseInc_;
        if (phase_ >= 1.0)
            phase_ -= 1.0;

        return out;
    }

private:
    static float polyBlep (float t, float dt) noexcept
    {
        if (dt <= 0.0f)
            return 0.0f;

        if (t < dt)
        {
            t /= dt;
            return t + t - t * t - 1.0f;
        }

        if (t > 1.0f - dt)
        {
            t = (t - 1.0f) / dt;
            return t * t + t + t + 1.0f;
        }

        return 0.0f;
    }

    double sampleRate_ = 44100.0;
    double phase_ = 0.0;
    float phaseInc_ = 0.0f;
    float integrator_ = 0.0f;
};

} // namespace pluggva::dsp
