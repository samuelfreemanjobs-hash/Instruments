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

void modulateDelay (const std::array<float, 4096>& buf,
                    int writeIndex,
                    float centerMs,
                    float swingMs,
                    float mod,
                    double sampleRate,
                    float& wetLp,
                    float& out) noexcept
{
    const float sr = static_cast<float> (sampleRate);
    const float centerSamples = centerMs * 0.001f * sr;
    const float halfSwing = 0.5f * swingMs * 0.001f * sr;
    const float d = centerSamples + halfSwing * mod;
    out = readDelayHermite (buf, writeIndex, d);
    out = onePoleLowpass (out, wetLp, 0.14f);
}
} // namespace

void BbdChorus::prepare (double sampleRate) noexcept
{
    sampleRate_ = sampleRate;
    rng_.setSeed (0xBD13u);
    reset();
}

void BbdChorus::reset() noexcept
{
    chorusPhaseI_ = 0.0f;
    chorusPhaseII_ = 0.25f;
    delayWrite_ = 0;
    delayL_.fill (0.0f);
    delayR_.fill (0.0f);
    wetLpL_ = 0.0f;
    wetLpR_ = 0.0f;
    clockNoiseLp_ = 0.0f;
}

void BbdChorus::process (float& left, float& right, int mode) noexcept
{
    if (mode <= 0)
        return;

    const float rateI = 0.393f;
    const float rateII = 0.797f;

    chorusPhaseI_ += (mode >= 2 ? rateII : rateI) / static_cast<float> (sampleRate_);
    if (chorusPhaseI_ >= 1.0f)
        chorusPhaseI_ -= 1.0f;
    chorusPhaseII_ += rateII / static_cast<float> (sampleRate_);
    if (chorusPhaseII_ >= 1.0f)
        chorusPhaseII_ -= 1.0f;

    const float mod1 = triangle01 (chorusPhaseI_);
    const float mod2 = triangle01 (chorusPhaseII_);

    delayL_[static_cast<std::size_t> (delayWrite_)] = left;
    delayR_[static_cast<std::size_t> (delayWrite_)] = right;

    float wL = 0.0f;
    float wR = 0.0f;
    if (mode == 3)
    {
        float aL = 0.0f;
        float aR = 0.0f;
        modulateDelay (delayL_, delayWrite_, 3.0f, 4.66f, mod1, sampleRate_, wetLpL_, aL);
        modulateDelay (delayR_, delayWrite_, 3.0f, 4.66f, -mod1, sampleRate_, wetLpR_, aR);
        float bL = 0.0f;
        float bR = 0.0f;
        float tmpLpL = wetLpL_;
        float tmpLpR = wetLpR_;
        modulateDelay (delayL_, delayWrite_, 3.0f, 4.0f, mod2, sampleRate_, tmpLpL, bL);
        modulateDelay (delayR_, delayWrite_, 3.0f, 4.0f, -mod2, sampleRate_, tmpLpR, bR);
        wL = 0.5f * (aL + bL);
        wR = 0.5f * (aR + bR);
    }
    else
    {
        const float centerMs = 3.0f;
        const float swingMs = mode >= 2 ? 4.0f : 4.66f;
        modulateDelay (delayL_, delayWrite_, centerMs, swingMs, mod1, sampleRate_, wetLpL_, wL);
        modulateDelay (delayR_, delayWrite_, centerMs, swingMs, -mod1, sampleRate_, wetLpR_, wR);
    }

    delayWrite_ = (delayWrite_ + 1) % static_cast<int> (delayL_.size());

    const float clockNoise = rng_.nextFloat() * 2.0f - 1.0f;
    clockNoiseLp_ += 0.06f * (clockNoise - clockNoiseLp_);
    wL += clockNoiseLp_ * 0.00022f;
    wR += clockNoiseLp_ * 0.00022f;

    constexpr float kDry = 0.863f;
    constexpr float kWet = 1.257f;
    const float norm = 1.0f / (kDry + kWet);
    left = norm * (kDry * left + kWet * wL);
    right = norm * (kDry * right + kWet * wR);
}
} // namespace junovax::dsp