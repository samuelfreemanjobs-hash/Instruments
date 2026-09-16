#pragma once

#include "DSP/AudioConstants.h"

#include <cmath>
#include <cstdint>

namespace jdupgraded::preset
{

/** Maps JD 0–127 envelope time indices to seconds (log-shaped, clean-room approximation). */
inline float jdEnvelopeTimeToSeconds (std::uint8_t jdTime) noexcept
{
    const float norm = static_cast<float> (jdTime & 0x7F) / 127.0f;
    constexpr float kMin = jdupgraded::dsp::kMinEnvelopeTimeSec;
    constexpr float kMax = 15.0f;
    return kMin * std::pow (kMax / kMin, norm);
}

inline float jdEnvelopeLevelToNorm (std::uint8_t jdLevel) noexcept
{
    return static_cast<float> (jdLevel & 0x7F) / 127.0f;
}

} // namespace jdupgraded::preset
