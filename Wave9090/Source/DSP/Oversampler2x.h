#pragma once

#include "DSP/DspUtils.h"

#include <algorithm>
#include <cmath>

namespace wave9090::dsp
{

/** Lightweight 2x oversampling (linear up / box down). */
class Oversampler2x final
{
public:
    template <typename ProcFn>
    float process (float input, ProcFn&& fn) noexcept
    {
        const float mid = 0.5f * (lastInput_ + input);
        lastInput_ = input;
        const float y0 = fn (mid);
        const float y1 = fn (input);
        return 0.5f * (y0 + y1);
    }

    void reset() noexcept { lastInput_ = 0.0f; }

private:
    float lastInput_ = 0.0f;
};

} // namespace wave9090::dsp