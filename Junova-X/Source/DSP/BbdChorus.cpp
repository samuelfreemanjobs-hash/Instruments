#include "BbdChorus.h"

namespace junovax::dsp
{
namespace
{
/** Triangle LFO in [-1, 1] (Juno chorus I/II). */
float triangle01 (float phase) noexcept
{
    return 1.0f - 4.0f * std::abs (phase - 0.5f);
}

float hermite4 (float y0, float y1, float y2, float y3, float t) noexcept
{
    const float c0 = y1;
    const float c1 = 0.5f * (y2 - y0);
    const float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
    const float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
    return ((c3 * t + c2) * t + c1) * t + c0;
}

float readDelayHermite (const std::array<float, 4096>& buf, int writeIndex, float delaySamples) noexcept
{
    const int cap = static_cast<int> (buf.size());
    delaySamples = juce::jmax (2.0f, delaySamples);
    const int base = static_cast<int> (delaySamples);
    const float frac = delaySamples - static_cast<float> (base);

    auto at = [&] (int offset) -> float
    {
        const int idx = (writeIndex - offset + cap) % cap;
        return buf[static_cast<std::size_t> (idx)];
    };

    return hermite4 (at (base + 1), at (base), at (base - 1), at (base - 2), frac);
}

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

    // LFO rates from published Juno hardware measurements (see REFERENCE_PLUGINS.md).
    const float rateHz = mode >= 3 ? 8.0f : mode >= 2 ? 0.797f : 0.393f;
    const float centerMs = 3.0f;
    const float swingMs = mode >= 3 ? 1.2f : mode >= 2 ? 4.0f : 4.66f;

    chorusPhase_ += rateHz / static_cast<float> (sampleRate_);
    if (chorusPhase_ >= 1.0f)
        chorusPhase_ -= 1.0f;

    float mod = mode >= 3 ? std::sin (chorusPhase_ * juce::MathConstants<float>::twoPi)
                          : triangle01 (chorusPhase_);

    const float sr = static_cast<float> (sampleRate_);
    const float centerSamples = centerMs * 0.001f * sr;
    const float halfSwing = 0.5f * swingMs * 0.001f * sr;
    const float dL = centerSamples + halfSwing * mod;
    const float dR = centerSamples - halfSwing * mod;

    delayL_[static_cast<std::size_t> (delayWrite_)] = left;
    delayR_[static_cast<std::size_t> (delayWrite_)] = right;

    float wL = readDelayHermite (delayL_, delayWrite_, dL);
    float wR = readDelayHermite (delayR_, delayWrite_, dR);
    delayWrite_ = (delayWrite_ + 1) % static_cast<int> (delayL_.size());

    const float lpCoeff = 0.12f;
    wL = onePoleLowpass (wL, wetLpL_, lpCoeff);
    wR = onePoleLowpass (wR, wetLpR_, lpCoeff);

    // IC6 summer-ish mix (hardware-calibrated ratios, peak-normalized).
    constexpr float kDry = 0.863f;
    constexpr float kWet = 1.257f;
    const float norm = 1.0f / (kDry + kWet);
    left = norm * (kDry * left + kWet * wL);
    right = norm * (kDry * right + kWet * wR);
}
} // namespace junovax::dsp
