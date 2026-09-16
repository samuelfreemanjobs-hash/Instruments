#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>

namespace jdupgraded::dsp
{

/** Fixed-buffer stereo delay + diffuse feedback (Group B spatial network, lightweight). */
class GroupBSpatial final
{
public:
    static constexpr std::size_t kMaxDelaySamples = 48000;

    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
        delaySamples_ = static_cast<std::size_t> (sampleRate_ * 0.028);
        delaySamples_ = std::min (delaySamples_, kMaxDelaySamples - 1);
        chorusInc_ = static_cast<float> (6.283185307179586 * 0.65 / sampleRate_);
        reset();
    }

    void reset() noexcept
    {
        buffer_.fill (0.0f);
        writeIndex_ = 0;
        lpState_ = 0.0f;
    }

    void setMix (float norm) noexcept { mix_ = std::clamp (norm, 0.0f, 1.0f); }

    void setChorus (float norm) noexcept { chorus_ = std::clamp (norm, 0.0f, 1.0f); }

    void processStereo (float& left, float& right) noexcept
    {
        if (mix_ <= 1.0e-5f)
            return;

        float modL = 0.0f;
        float modR = 0.0f;
        if (chorus_ > 1.0e-5f)
        {
            chorusPhase_ += chorusInc_;
            if (chorusPhase_ > 6.283185307179586f)
                chorusPhase_ -= 6.283185307179586f;
            const float depthSamples = chorus_ * 16.0f;
            modL = std::sin (chorusPhase_) * depthSamples;
            modR = std::sin (chorusPhase_ + 1.5707963267948966f) * depthSamples;
        }

        const auto readL = modulatedReadIndex (delaySamples_, modL);
        const auto readR = modulatedReadIndex (delaySamples_, modR);
        const float delayed = 0.5f * (buffer_[readL] + buffer_[readR]);

        lpState_ += 0.12f * (delayed - lpState_);
        const float diffuse = lpState_;

        buffer_[writeIndex_] = left + diffuse * 0.35f;
        writeIndex_ = (writeIndex_ + 1) % kMaxDelaySamples;

        const float wetL = left + diffuse * 0.55f;
        const float wetR = right - diffuse * 0.45f;

        left = left * (1.0f - mix_) + wetL * mix_;
        right = right * (1.0f - mix_) + wetR * mix_;
    }

private:
    std::size_t modulatedReadIndex (std::size_t baseDelay, float modSamples) const noexcept
    {
        const float delayed = static_cast<float> (baseDelay) + modSamples;
        const auto clamped = static_cast<std::size_t> (std::max (1.0f, std::min (delayed, static_cast<float> (kMaxDelaySamples - 2))));
        return (writeIndex_ + kMaxDelaySamples - clamped) % kMaxDelaySamples;
    }

    double sampleRate_ = 44100.0;
    std::size_t delaySamples_ = 1200;
    std::size_t writeIndex_ = 0;
    float mix_ = 0.0f;
    float chorus_ = 0.0f;
    float chorusPhase_ = 0.0f;
    float chorusInc_ = 0.0001f;
    float lpState_ = 0.0f;
    std::array<float, kMaxDelaySamples> buffer_{};
};

} // namespace jdupgraded::dsp
