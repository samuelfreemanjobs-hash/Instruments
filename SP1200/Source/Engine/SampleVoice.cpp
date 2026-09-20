#include "SampleVoice.h"

#include <algorithm>
#include <cmath>

namespace sp1200
{
void SampleVoice::start (const TwelveBitBuffer* data, float velocity, float tuneSemitones, float level, float pan)
{
    data_ = data;
    phase_ = 0.0;
    phaseInc_ = std::pow (2.0, static_cast<double> (tuneSemitones) / 12.0);
    level_ = level * std::clamp (velocity, 0.0f, 1.0f);
    const float norm = std::clamp ((pan + 1.0f) * 0.5f, 0.0f, 1.0f);
    panL_ = std::sqrt (1.0f - norm);
    panR_ = std::sqrt (norm);
    active_ = data_ != nullptr && data_->size() > 0;
}

void SampleVoice::forceStop() noexcept
{
    active_ = false;
    data_ = nullptr;
}

float SampleVoice::renderNextSample() noexcept
{
    if (! active_ || data_ == nullptr)
        return 0.0f;

    const auto len = static_cast<double> (data_->size());
    if (phase_ >= len)
    {
        active_ = false;
        return 0.0f;
    }

    const auto idx = static_cast<std::size_t> (phase_);
    const auto s12 = data_->getSample (idx);
    const float sample = (static_cast<float> (s12) - 2048.0f) / 2048.0f;
    phase_ += phaseInc_;
    return sample * level_;
}

} // namespace sp1200
