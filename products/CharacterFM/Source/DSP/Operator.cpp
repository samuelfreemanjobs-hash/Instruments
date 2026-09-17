#include "Operator.h"

#include <algorithm>

namespace characterfm::dsp
{
namespace
{
constexpr float kTwoPi = 6.28318530718f;

float timeToCoeff (float seconds, double sampleRate) noexcept
{
    seconds = std::max (seconds, 0.0005f);
    return 1.0f - std::exp (-1.0f / (seconds * static_cast<float> (sampleRate)));
}
} // namespace

void Operator::reset (double sampleRate) noexcept
{
    sampleRate_ = sampleRate;
    stage_ = Stage::idle;
    envelope_ = 0.0f;
    phase_ = 0.0f;
    lastOutput_ = 0.0f;
}

void Operator::setParams (const OperatorParams& p) noexcept
{
    params_ = p;
    updatePhaseIncrement();
}

void Operator::start (float velocity) noexcept
{
    velocity_ = std::clamp (velocity, 0.0f, 1.0f);
    stage_ = Stage::attack;
    envelope_ = 0.0f;
    phase_ = 0.0f;
}

void Operator::stop() noexcept
{
    if (stage_ != Stage::idle)
        stage_ = Stage::release;
}

void Operator::updatePhaseIncrement() noexcept
{
    const float freq = baseFrequencyHz_ * params_.ratio;
    phaseInc_ = kTwoPi * freq / static_cast<float> (sampleRate_);
}

void Operator::advanceEnvelope() noexcept
{
    switch (stage_)
    {
        case Stage::attack:
        {
            const float c = timeToCoeff (params_.attackSec, sampleRate_);
            envelope_ += (1.0f - envelope_) * c;
            if (envelope_ >= 0.999f)
            {
                envelope_ = 1.0f;
                stage_ = Stage::decay;
            }
            break;
        }
        case Stage::decay:
        {
            const float c = timeToCoeff (params_.decaySec, sampleRate_);
            envelope_ += (params_.sustain - envelope_) * c;
            if (std::abs (envelope_ - params_.sustain) < 0.001f)
            {
                envelope_ = params_.sustain;
                stage_ = Stage::sustain;
            }
            break;
        }
        case Stage::sustain:
            envelope_ = params_.sustain;
            break;
        case Stage::release:
        {
            const float c = timeToCoeff (params_.releaseSec, sampleRate_);
            envelope_ += (0.0f - envelope_) * c;
            if (envelope_ <= 0.001f)
            {
                envelope_ = 0.0f;
                stage_ = Stage::idle;
            }
            break;
        }
        case Stage::idle:
        default:
            envelope_ = 0.0f;
            break;
    }
}

float Operator::render (float phaseMod) noexcept
{
    if (stage_ == Stage::idle)
    {
        lastOutput_ = 0.0f;
        return 0.0f;
    }

    advanceEnvelope();

    const float modulatedPhase = phase_ + phaseMod;
    const float s = std::sin (modulatedPhase);
    lastOutput_ = s * envelope_ * params_.level * velocity_;

    phase_ += phaseInc_;
    if (phase_ > kTwoPi)
        phase_ -= kTwoPi;

    return lastOutput_;
}

} // namespace characterfm::dsp
