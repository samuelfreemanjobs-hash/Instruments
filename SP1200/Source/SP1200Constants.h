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

/** Korg SQ-1 default map (1-based MIDI channel). */
inline constexpr int kDefaultMidiChannel = 10;
inline constexpr int kPadNoteStart = 36;
inline constexpr int kPadNoteEnd = 52; // exclusive (36–51)
inline constexpr int kFaderCcStart = 20;
inline constexpr int kFaderCcEnd = 36; // exclusive (20–35 → pads 0–15)

inline constexpr int kSongSlotEnd = -1;
inline constexpr int kNoChokeGroup = -1;
/** Default open/closed hat choke (pads 5–6 and 13–14). */
inline constexpr int kDefaultHatChokeGroup = 1;
inline constexpr int kMaxChokeGroups = 4;

/** 33 → 45 RPM vinyl trick: import as if ~+36% faster, then tune down. */
inline constexpr double kVinylImportRateMultiplier = 45.0 / 33.0;
inline constexpr float kVinylImportTuneDownSemitones = -5.02f; // 12*log2(33/45)

/** Default multi-pitch semitone offsets (pad index → offset). */
inline constexpr float kDefaultMultiPitchOffsets[kMultiPitchSlots] = {
    0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 7.0f, 9.0f,
    12.0f, -5.0f, -3.0f, -2.0f, 10.0f, 14.0f, -7.0f, -12.0f
};

} // namespace sp1200
