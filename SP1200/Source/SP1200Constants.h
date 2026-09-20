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

inline constexpr int kMaxPatterns = 99;
inline constexpr int kMinPatternBars = 1;
inline constexpr int kMaxPatternBars = 4;
inline constexpr int kStepsPerBar = 16; // 1/16 grid
inline constexpr int kMultiPitchSlots = 16;

inline constexpr int kDefaultMidiChannel = 10;
inline constexpr int kPadNoteStart = 36;

/** Default multi-pitch semitone offsets (pad index → offset). */
inline constexpr float kDefaultMultiPitchOffsets[kMultiPitchSlots] = {
    0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 7.0f, 9.0f,
    12.0f, -5.0f, -3.0f, -2.0f, 10.0f, 14.0f, -7.0f, -12.0f
};

} // namespace sp1200
