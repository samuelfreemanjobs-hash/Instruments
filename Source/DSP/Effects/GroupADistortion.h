#pragma once

#include <algorithm>
#include <cmath>

namespace jdupgraded::dsp
{

/** Seven-stage soft clipping ladder (Group A character, no heap). */
class GroupADistortion final
{
public:
    void setDrive (float norm) noexcept
    {
        drive_ = std::clamp (norm, 0.0f, 1.0f);
    }

    float process (float x) noexcept
    {
        if (drive_ <= 1.0e-5f)
            return x;

        const float pre = 1.0f + drive_ * 6.0f;
        x *= pre;

        for (int stage = 0; stage < 7; ++stage)
            x = softClip (x);

        return x * (1.0f / (1.0f + drive_ * 2.0f));
    }

private:
    static float softClip (float v) noexcept
    {
        return std::tanh (v);
    }

    float drive_ = 0.0f;
};

} // namespace jdupgraded::dsp
