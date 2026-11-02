#pragma once

#include <algorithm>
#include <cmath>

namespace jdupgraded::dsp
{

/** Group A macro: distortion ladder + optional phaser smear (no heap). */
class GroupADistortion final
{
public:
    void setDrive (float norm) noexcept { drive_ = std::clamp (norm, 0.0f, 1.0f); }

    void setPhaserMix (float norm) noexcept { phaserMix_ = std::clamp (norm, 0.0f, 1.0f); }

    float process (float x) noexcept
    {
        if (drive_ > 1.0e-5f)
        {
            const float pre = 1.0f + drive_ * 6.0f;
            x *= pre;

            for (int stage = 0; stage < 7; ++stage)
                x = softClip (x);

            x *= (1.0f / (1.0f + drive_ * 2.0f));
        }

        if (phaserMix_ <= 1.0e-5f)
            return x;

        phaserPhase_ += 0.015f;
        if (phaserPhase_ > 6.283185307179586f)
            phaserPhase_ -= 6.283185307179586f;

        const float mod = std::sin (phaserPhase_);
        const float phased = x * (0.65f + 0.35f * mod);
        return x * (1.0f - phaserMix_) + phased * phaserMix_;
    }

private:
    static float softClip (float v) noexcept { return std::tanh (v); }

    float drive_ = 0.0f;
    float phaserMix_ = 0.0f;
    float phaserPhase_ = 0.0f;
};

} // namespace jdupgraded::dsp
