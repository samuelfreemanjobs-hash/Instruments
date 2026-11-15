#pragma once

#include <cmath>

namespace wave9090::dsp
{

/** PolyBLEP residual for naive sawtooth segments (band-limited discontinuity). */
inline float polyBlep (double phaseInc, double phase01) noexcept
{
    if (phaseInc <= 0.0)
        return 0.0f;

  const double t = phase01;
    if (t < phaseInc)
    {
        const double x = t / phaseInc;
        return static_cast<float> (x + x - x * x - 1.0);
    }
    if (t > 1.0 - phaseInc)
    {
        const double x = (t - 1.0) / phaseInc;
        return static_cast<float> (x * x + x + x + 1.0);
    }
    return 0.0f;
}

} // namespace wave9090::dsp