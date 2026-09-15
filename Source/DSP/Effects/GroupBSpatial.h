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
        reset();
    }

    void reset() noexcept
    {
        buffer_.fill (0.0f);
        writeIndex_ = 0;
        lpState_ = 0.0f;
    }

    void setMix (float norm) noexcept { mix_ = std::clamp (norm, 0.0f, 1.0f); }

    void processStereo (float& left, float& right) noexcept
    {
        if (mix_ <= 1.0e-5f)
            return;

        const auto readIndex = (writeIndex_ + kMaxDelaySamples - delaySamples_) % kMaxDelaySamples;
        const float delayed = buffer_[readIndex];

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
    double sampleRate_ = 44100.0;
    std::size_t delaySamples_ = 1200;
    std::size_t writeIndex_ = 0;
    float mix_ = 0.0f;
    float lpState_ = 0.0f;
    std::array<float, kMaxDelaySamples> buffer_{};
};

} // namespace jdupgraded::dsp
