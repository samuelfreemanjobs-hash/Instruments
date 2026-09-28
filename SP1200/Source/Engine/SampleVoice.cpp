#include "SampleVoice.h"

#include <algorithm>
#include <cmath>

namespace sp1200
{
void SampleVoice::start (const TwelveBitBuffer* data,
                       float velocity,
                       float tuneSemitones,
                       float level,
                       float pan,
                       std::size_t startSample,
                       std::size_t endSample,
                       float decayNorm,
                       double hostSampleRate,
                       int padIndex,
                       VoiceFilterRole filterRole)
{
    data_ = data;
    startSample_ = startSample;
    phase_ = static_cast<double> (startSample_);
    const auto dataLen = data_ != nullptr ? data_->size() : 0u;
    const auto end = endSample > startSample && endSample <= dataLen ? endSample : dataLen;
    endPhase_ = static_cast<double> (end);
    phaseInc_ = std::pow (2.0, static_cast<double> (tuneSemitones) / 12.0);
    level_ = level * std::clamp (velocity, 0.0f, 1.0f);
    const float norm = std::clamp ((pan + 1.0f) * 0.5f, 0.0f, 1.0f);
    panL_ = std::sqrt (1.0f - norm);
    panR_ = std::sqrt (norm);
    padIndex_ = padIndex;
    filterRole_ = filterRole;
    hpfLpState_ = 0.0f;
    const double sr = std::max (hostSampleRate, 8000.0);
    hpfCoef_ = static_cast<float> (std::exp (-2.0 * 3.141592653589793 * 2800.0 / sr));
    ampEnv_ = 1.0f;
    const float d = std::clamp (decayNorm, 0.0f, 1.0f);
    const double releaseSec = 0.02 + static_cast<double> (d) * 2.5;
    ampDecayCoeff_ = static_cast<float> (std::exp (-1.0 / (releaseSec * sr)));
    active_ = data_ != nullptr && endPhase_ > phase_;
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

    if (phase_ >= endPhase_)
    {
        active_ = false;
        return 0.0f;
    }

    const auto idx = static_cast<std::size_t> (phase_);
    const auto s12 = data_->getSample (idx);
    const float sample = (static_cast<float> (s12) - 2048.0f) / 2048.0f;
    phase_ += phaseInc_;
    float shaped = sample;
    if (filterRole_ == VoiceFilterRole::hiTrim)
    {
        hpfLpState_ += hpfCoef_ * (shaped - hpfLpState_);
        shaped = shaped - hpfLpState_;
    }

    const float out = shaped * level_ * ampEnv_;
    ampEnv_ *= ampDecayCoeff_;
    return out;
}

} // namespace sp1200
