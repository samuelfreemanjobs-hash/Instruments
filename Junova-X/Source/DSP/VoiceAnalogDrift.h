#pragma once

#include <juce_core/juce_core.h>

namespace junovax::dsp
{
/** Slow per-voice pitch/cutoff wander (scaled by DRIFT). */
struct VoiceAnalogDrift
{
    void seed (juce::Random& rng) noexcept
    {
        pitchCents_ = (rng.nextFloat() * 2.0f - 1.0f) * 4.0f;
        cutoffNorm_ = (rng.nextFloat() * 2.0f - 1.0f) * 0.08f;
        pitchTarget_ = pitchCents_;
        cutoffTarget_ = cutoffNorm_;
        wanderPhase_ = rng.nextFloat();
    }

    void advance (double sampleRate, float driftAmount, juce::Random& rng) noexcept
    {
        const float rate = 0.12f + driftAmount * 0.004f;
        wanderPhase_ += rate / static_cast<float> (sampleRate);
        if (wanderPhase_ >= 1.0f)
        {
            wanderPhase_ -= 1.0f;
            pitchTarget_ = (rng.nextFloat() * 2.0f - 1.0f) * (0.8f + driftAmount * 0.22f);
            cutoffTarget_ = (rng.nextFloat() * 2.0f - 1.0f) * (0.015f + driftAmount * 0.0045f);
        }

        const float coeff = 1.0f - std::exp (-0.7f / static_cast<float> (sampleRate));
        pitchCents_ += (pitchTarget_ - pitchCents_) * coeff;
        cutoffNorm_ += (cutoffTarget_ - cutoffNorm_) * coeff;
    }

    float pitchRatio (float driftAmount) const noexcept
    {
        const float cents = pitchCents_ * (driftAmount * 0.01f);
        return std::pow (2.0f, cents / 1200.0f);
    }

    float cutoffMul (float driftAmount) const noexcept
    {
        return std::pow (2.0f, cutoffNorm_ * (driftAmount * 0.01f) * 1.75f);
    }

private:
    float pitchCents_ = 0.0f;
    float cutoffNorm_ = 0.0f;
    float pitchTarget_ = 0.0f;
    float cutoffTarget_ = 0.0f;
    float wanderPhase_ = 0.0f;
};
} // namespace junovax::dsp