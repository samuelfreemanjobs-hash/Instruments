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

/** JD pitch envelope / bipolar level bytes: 0x32 is neutral (unity pitch multiplier). */
inline constexpr int kJdPitchLevelCenter = 0x32;

inline float jdPitchLevelToMultiplier (std::uint8_t jdLevel) noexcept
{
    const int delta = static_cast<int> (jdLevel & 0x7F) - kJdPitchLevelCenter;
    const float semis = static_cast<float> (delta) * (24.0f / 50.0f);
    return std::pow (2.0f, semis / 12.0f);
}

/** LFO rate byte → Hz (clean-room log map). */
inline float jdLfoRateToHz (std::uint8_t jdRate) noexcept
{
    const float norm = static_cast<float> (jdRate & 0x7F) / 127.0f;
    return 0.05f * std::pow (400.0f, norm);
}

/** LFO sensitivity byte → peak pitch modulation in semitones. */
inline float jdLfoSensToPitchDepthSemis (std::uint8_t jdSens) noexcept
{
    const int delta = static_cast<int> (jdSens & 0x7F) - kJdPitchLevelCenter;
    return static_cast<float> (delta) * (12.0f / 50.0f);
}

} // namespace jdupgraded::preset
