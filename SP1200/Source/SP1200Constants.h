#pragma once

#include <cstdint>

namespace sp1200
{
/** Internal DSP rate — UI always displays "26.040 kHz". */
inline constexpr double kSampleRateHz = 26040.0;

inline constexpr std::int64_t kMaxMemorySeconds = 420; // 7:00
inline constexpr std::int64_t kMaxMemorySamples =
    static_cast<std::int64_t> (kSampleRateHz) * kMaxMemorySeconds;

inline constexpr int kNumPads = 16;
inline constexpr int kNumVoices = 16;
inline constexpr int kNumBanks = 4;

inline constexpr int kDefaultMidiChannel = 10; // SQ-1 map (1-based display → 10)
inline constexpr int kPadNoteStart = 36;       // C2

} // namespace sp1200
