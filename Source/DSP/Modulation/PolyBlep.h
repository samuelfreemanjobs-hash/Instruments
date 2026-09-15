#pragma once

#include <cmath>

namespace jdupgraded::dsp
{

/** PolyBLEP residual for hard-sync discontinuities (t in [0,1), dt = phase increment / cycle length). */
inline float polyBlep (double t, double dt) noexcept
{
    if (dt <= 0.0)
        return 0.0f;

    if (t < dt)
    {
        const double x = t / dt;
        return static_cast<float> (x + x - x * x - 1.0);
    }

    if (t > 1.0 - dt)
    {
        const double x = (t - 1.0) / dt;
        return static_cast<float> (x * x + x + x + 1.0);
    }

    return 0.0f;
}

} // namespace jdupgraded::dsp
