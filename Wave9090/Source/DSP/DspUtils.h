#pragma once

#include <algorithm>
#include <cmath>

namespace wave9090::dsp
{

inline float flushDenormal (float x) noexcept
{
    return x + 1.0e-18f;
}

/** One-pole smoother toward target (time in seconds). */
class ParamSmoother final
{
public:
    void prepare (double sampleRate) noexcept { sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0; }

    void reset (float value) noexcept
    {
        current_ = value;
        target_ = value;
    }

    void setTarget (float value) noexcept { target_ = value; }

    void setTimeMs (float ms) noexcept
    {
        ms = std::max (0.05f, ms);
        coef_ = 1.0f - std::exp (-1.0f / (static_cast<float> (sampleRate_) * ms * 0.001f));
    }

    float process() noexcept
    {
        current_ += (target_ - current_) * coef_;
        return current_;
    }

    float getCurrent() const noexcept { return current_; }

private:
    double sampleRate_ = 44100.0;
    float current_ = 0.0f;
    float target_ = 0.0f;
    float coef_ = 1.0f;
};

/** DC blocking high-pass (~10 Hz). */
class DcBlocker final
{
public:
    void prepare (double sampleRate) noexcept
    {
        const float fc = 10.0f;
        const float r = 1.0f - (2.0f * 3.14159265f * fc) / static_cast<float> (sampleRate > 0.0 ? sampleRate : 44100.0);
        coeff_ = std::clamp (r, 0.0f, 0.9999f);
    }

    void reset() noexcept
    {
        x1_ = 0.0f;
        y1_ = 0.0f;
    }

    float process (float x) noexcept
    {
        const float y = x - x1_ + coeff_ * y1_;
        x1_ = x;
        y1_ = y;
        return flushDenormal (y);
    }

private:
    float coeff_ = 0.995f;
    float x1_ = 0.0f;
    float y1_ = 0.0f;
};

} // namespace wave9090::dsp