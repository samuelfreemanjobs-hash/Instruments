#pragma once

#include <cstddef>
#include <cstdint>

namespace jdupgraded::dsp
{

#ifndef JDUPGRADED_CONTROL_RATE_DIVISOR
#define JDUPGRADED_CONTROL_RATE_DIVISOR 32
#endif

inline constexpr std::size_t kControlRateDivisor = JDUPGRADED_CONTROL_RATE_DIVISOR;
inline constexpr std::size_t kTonesPerVoice = 4;
inline constexpr std::size_t kMaxPolyphony = 32;
inline constexpr std::size_t kMaxPcmChannels = 1;

inline constexpr float kMinEnvelopeTimeSec = 0.001f;
inline constexpr float kMaxEnvelopeTimeSec = 60.0f;

} // namespace jdupgraded::dsp
