#include "BbdChorus.h"

namespace junovax::dsp
{
namespace
{
float onePoleLowpass (float in, float& state, float coeff) noexcept
{
    state += coeff * (in - state);
    return state;
}
} // namespace

void BbdChorus::prepare (double sampleRate) noexcept
{
    sampleRate_ = sampleRate;
    reset();
}

void BbdChorus::reset() noexcept
{
    chorusPhase_ = 0.0f;
    chorusPhase2_ = 0.0f;
    delayWrite_ = 0;
    delayL_.fill (0.0f);
    delayR_.fill (0.0f);
    wetLpL_ = 0.0f;
    wetLpR_ = 0.0f;
}

void BbdChorus::process (float& left, float& right, int mode) noexcept
{
    if (mode <= 0)
        return;

    const float rateHz = mode >= 3 ? 0.65f : mode >= 2 ? 0.82f : 0.42f;
    const float depthMs = mode >= 3 ? 3.2f : mode >= 2 ? 2.6f : 2.1f;
    const float depth = depthMs * 0.001f * static_cast<float> (sampleRate_);

    chorusPhase_ += rateHz / static_cast<float> (sampleRate_);
    if (chorusPhase_ > 1.0f)
        chorusPhase_ -= 1.0f;

    float mod = std::sin (chorusPhase_ * juce::MathConstants<float>::twoPi);
    if (mode >= 3)
    {
        chorusPhase2_ += 1.05f / static_cast<float> (sampleRate_);
        if (chorusPhase2_ > 1.0f)
            chorusPhase2_ -= 1.0f;
        mod = 0.55f * mod + 0.45f * std::sin (chorusPhase2_ * juce::MathConstants<float>::twoPi);
    }

    const int dL = static_cast<int> (depth * (1.0f + 0.85f * mod));
    const int dR = static_cast<int> (depth * (1.0f - 0.85f * mod));
    const int cap = static_cast<int> (delayL_.size());
    const int readL = (delayWrite_ - dL + cap) % cap;
    const int readR = (delayWrite_ - dR + cap) % cap;

    delayL_[static_cast<std::size_t> (delayWrite_)] = left;
    delayR_[static_cast<std::size_t> (delayWrite_)] = right;
    delayWrite_ = (delayWrite_ + 1) % cap;

    const float wet = mode >= 3 ? 0.42f : mode >= 2 ? 0.36f : 0.33f;
    const float lpCoeff = 0.08f;
    float wL = delayL_[static_cast<std::size_t> (readL)];
    float wR = delayR_[static_cast<std::size_t> (readR)];
    wL = onePoleLowpass (wL, wetLpL_, lpCoeff);
    wR = onePoleLowpass (wR, wetLpR_, lpCoeff);

    left = left * (1.0f - wet) + wL * wet;
    right = right * (1.0f - wet) + wR * wet;
}
} // namespace junovax::dsp
