#include "BbdChorus.h"

namespace junovax::dsp
{
void BbdChorus::prepare (double sampleRate) noexcept
{
    sampleRate_ = sampleRate;
    reset();
}

void BbdChorus::reset() noexcept
{
    chorusPhase_ = 0.0f;
    delayWrite_ = 0;
    delayL_.fill (0.0f);
    delayR_.fill (0.0f);
}

void BbdChorus::process (float& left, float& right, int mode) noexcept
{
    if (mode <= 0)
        return;

    const float depth = (mode >= 3 ? 0.0035f : mode >= 2 ? 0.003f : 0.0025f) * static_cast<float> (sampleRate_);
    chorusPhase_ += (mode >= 3 ? 0.9f : 0.55f) / static_cast<float> (sampleRate_);
    if (chorusPhase_ > 1.0f)
        chorusPhase_ -= 1.0f;
    const float mod = std::sin (chorusPhase_ * juce::MathConstants<float>::twoPi);

    const int dL = static_cast<int> (depth * (1.0f + mod));
    const int dR = static_cast<int> (depth * (1.0f - mod));
    const int cap = static_cast<int> (delayL_.size());
    const int readL = (delayWrite_ - dL + cap) % cap;
    const int readR = (delayWrite_ - dR + cap) % cap;

    delayL_[static_cast<std::size_t> (delayWrite_)] = left;
    delayR_[static_cast<std::size_t> (delayWrite_)] = right;
    delayWrite_ = (delayWrite_ + 1) % cap;

    const float wet = mode >= 2 ? 0.4f : 0.32f;
    left = left * (1.0f - wet) + delayL_[static_cast<std::size_t> (readL)] * wet;
    right = right * (1.0f - wet) + delayR_[static_cast<std::size_t> (readR)] * wet;
}
} // namespace junovax::dsp
