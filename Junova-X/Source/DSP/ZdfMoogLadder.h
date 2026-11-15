#pragma once

#include <juce_core/juce_core.h>

#include <algorithm>
#include <array>
#include <cmath>

namespace junovax::dsp
{
/**
 * Custom Huovilainen five-one-pole Moog ladder with tanh saturators (not juce::dsp::LadderFilter).
 * Resonance feedback on the 4th saturation node; stable self-oscillation near res=1.
 */
class ZdfMoogLadder
{
public:
    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
        cutoffScaler_ = static_cast<float> (-2.0 * juce::MathConstants<double>::pi / sampleRate_);
        reset();
    }

    void reset() noexcept
    {
        stages_.fill (0.0f);
        cutoffTransform_ = 0.984f;
        scaledResonance_ = 0.1f;
    }

    void setTargets (float cutoffHz, float resonance01, float drive) noexcept
    {
        cutoffHz = std::max (20.0f, cutoffHz);
        cutoffTransform_ = std::exp (cutoffHz * cutoffScaler_);
        scaledResonance_ = juce::jmap (juce::jlimit (0.0f, 1.0f, resonance01), 0.0f, 1.0f, 0.1f, 1.22f);
        setDriveCoeffs (std::max (1.0f, drive));
    }

    float process (float in, float excitation = 0.0f) noexcept
    {
        const float a1 = cutoffTransform_;
        const float g = a1 * -1.0f + 1.0f;
        const float b0 = g * 0.76923076923f;
        const float b1 = g * 0.23076923076f;

        const float vin = in + excitation;
        const float dx = gain_ * fastTanh (drive_ * vin);
        const float sat4 = fastTanh (drive2_ * stages_[4]);
        const float a = dx + scaledResonance_ * -4.0f * (gain2_ * sat4 - dx * 0.5f);

        const float b = b1 * stages_[0] + a1 * stages_[1] + b0 * a;
        const float c = b1 * stages_[1] + a1 * stages_[2] + b0 * b;
        const float d = b1 * stages_[2] + a1 * stages_[3] + b0 * c;
        const float e = b1 * stages_[3] + a1 * stages_[4] + b0 * d;

        stages_[0] = a;
        stages_[1] = b;
        stages_[2] = c;
        stages_[3] = d;
        stages_[4] = e;

        return e * 1.2f;
    }

private:
    void setDriveCoeffs (float drive) noexcept
    {
        drive_ = drive;
        const float d2 = drive_ * 0.04f + 0.96f;
        drive2_ = d2;
        gain_ = std::pow (drive_, -2.642f) * 0.6103f + 0.3903f;
        gain2_ = std::pow (d2, -2.642f) * 0.6103f + 0.3903f;
    }

    static float fastTanh (float x) noexcept
    {
        return std::tanh (x);
    }

    double sampleRate_ = 44100.0;
    float cutoffScaler_ = -0.000142f;
    float cutoffTransform_ = 0.984f;
    float scaledResonance_ = 0.1f;
    float drive_ = 1.2f;
    float drive2_ = 1.18f;
    float gain_ = 1.0f;
    float gain2_ = 1.0f;
    std::array<float, 5> stages_{};
};
} // namespace junovax::dsp