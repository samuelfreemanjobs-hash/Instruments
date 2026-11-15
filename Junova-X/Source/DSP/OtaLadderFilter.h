#pragma once

#include <algorithm>
#include <cmath>

namespace junovax::dsp
{
/** 4-pole ZDF ladder (OTA-style soft saturation). */
class OtaLadderFilter
{
public:
    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
        reset();
    }

    void reset() noexcept
    {
        z1_ = z2_ = z3_ = z4_ = 0.0f;
        cutoffSm_ = 1000.0f;
        resSm_ = 0.2f;
    }

    void setSampleRate (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate;
    }

    void setTargets (float cutoffHz, float resonance, float drive) noexcept
    {
        cutoffHz = std::max (20.0f, cutoffHz);
        const float coeff = 1.0f - std::exp (-2.0f * 3.14159265f * 8000.0f / static_cast<float> (sampleRate_));
        cutoffSm_ += (cutoffHz - cutoffSm_) * coeff;
        resSm_ += (std::clamp (resonance, 0.0f, 1.0f) - resSm_) * coeff;
        drive_ = std::max (1.0f, drive);
    }

    float processSample (float in) noexcept
    {
        const float g = std::tan (3.14159265f * cutoffSm_ / static_cast<float> (sampleRate_));
        const float k = 4.0f * resSm_;
        const float a1 = 1.0f / (1.0f + g);
        const float G = g * a1;

        const float x = std::tanh (in * drive_);
        float y1 = z1_ + G * (x - k * z4_ - z1_);
        float y2 = z2_ + G * (y1 - z2_);
        float y3 = z3_ + G * (y2 - z3_);
        float y4 = z4_ + G * (y3 - z4_);

        const float sat = 1.0f / (1.0f + 0.15f * std::abs (y4));
        y4 *= sat;

        z1_ = y1;
        z2_ = y2;
        z3_ = y3;
        z4_ = y4;
        return y4;
    }

private:
    double sampleRate_ = 44100.0;
    float z1_ = 0.0f;
    float z2_ = 0.0f;
    float z3_ = 0.0f;
    float z4_ = 0.0f;
    float cutoffSm_ = 1000.0f;
    float resSm_ = 0.2f;
    float drive_ = 1.0f;
};
} // namespace junovax::dsp