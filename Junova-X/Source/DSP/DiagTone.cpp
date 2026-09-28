#include "DiagTone.h"

#include <cmath>

namespace junovax::dsp
{
void DiagTone::prepare (double sampleRate) noexcept
{
    sampleRate_ = sampleRate > 0.0 ? sampleRate : 48000.0;
    reset();
}

void DiagTone::reset() noexcept
{
    phase_ = 0.0f;
}

float DiagTone::processSample() noexcept
{
    if (! enabled_)
        return 0.0f;

    constexpr float twoPi = 6.28318530718f;
    const float inc = twoPi * frequencyHz_ / static_cast<float> (sampleRate_);
    phase_ += inc;
    if (phase_ >= twoPi)
        phase_ -= twoPi;

    return 0.15f * std::sin (phase_);
}
} // namespace junovax::dsp
