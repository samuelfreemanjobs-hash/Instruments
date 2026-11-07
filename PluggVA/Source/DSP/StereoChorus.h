#pragma once

#include <algorithm>
#include <array>
#include <cmath>

namespace pluggva::dsp
{

class StereoChorus final
{
public:
    static constexpr int kMaxDelay = 4096;

    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
        bufferL_.fill (0.0f);
        bufferR_.fill (0.0f);
        writePos_ = 0;
        lfoPhase_ = 0.0;
    }

    void setAmount (float amount) noexcept
    {
        amount_ = std::clamp (amount, 0.0f, 1.0f);
    }

    void processBlock (float* left, float* right, int numSamples) noexcept
    {
        if (amount_ <= 0.0001f)
            return;

        const float baseDelay = static_cast<float> (sampleRate_) * 0.018f;
        const float depth = static_cast<float> (sampleRate_) * 0.004f * amount_;
        const float lfoHz = 0.35f + amount_ * 0.9f;

        for (int i = 0; i < numSamples; ++i)
        {
            lfoPhase_ += lfoHz / sampleRate_;
            if (lfoPhase_ >= 1.0)
                lfoPhase_ -= 1.0;

            const float mod = static_cast<float> (std::sin (lfoPhase_ * 6.283185307)) * depth;
            const float delayL = baseDelay + mod;
            const float delayR = baseDelay - mod;

            bufferL_[static_cast<std::size_t> (writePos_)] = left[i];
            bufferR_[static_cast<std::size_t> (writePos_)] = right[i];

            left[i] = left[i] * (1.0f - amount_ * 0.35f) + readDelay (bufferL_, delayL) * amount_ * 0.65f;
            right[i] = right[i] * (1.0f - amount_ * 0.35f) + readDelay (bufferR_, delayR) * amount_ * 0.65f;

            writePos_ = (writePos_ + 1) % kMaxDelay;
        }
    }

private:
    float readDelay (const std::array<float, kMaxDelay>& buf, float delaySamples) const noexcept
    {
        delaySamples = std::clamp (delaySamples, 1.0f, static_cast<float> (kMaxDelay - 2));
        const float readPos = static_cast<float> (writePos_) - delaySamples;
        const int idx0 = (static_cast<int> (readPos) + kMaxDelay) % kMaxDelay;
        const int idx1 = (idx0 + 1) % kMaxDelay;
        const float frac = readPos - std::floor (readPos);
        return buf[static_cast<std::size_t> (idx0)] * (1.0f - frac) + buf[static_cast<std::size_t> (idx1)] * frac;
    }

    double sampleRate_ = 44100.0;
    float amount_ = 0.0f;
    double lfoPhase_ = 0.0;
    int writePos_ = 0;
    std::array<float, kMaxDelay> bufferL_{};
    std::array<float, kMaxDelay> bufferR_{};
};

} // namespace pluggva::dsp
