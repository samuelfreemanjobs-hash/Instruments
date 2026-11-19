#pragma once

#include "SampleTypes.h"
#include <JuceHeader.h>
#include <array>

namespace huckaby
{

static constexpr int kNumTracks = 8;
static constexpr int kMaxSteps = 64;
static constexpr int kMaxBankSize = 16;
static constexpr int kMaxVoices = 32;

enum class SampleSelectionMode : int
{
    Static = 0,
    ModuloCycle = 1,
    Random = 2,
};

struct TrackState final
{
    int activeStepLength { 16 };
    int triggerCount { 0 };

    std::array<uint8_t, kMaxSteps> triggers {};
    std::array<float, kMaxSteps> velocities {};

    SampleSelectionMode selectionMode { SampleSelectionMode::Static };
    int staticBankIndex { 0 };
    int moduloDivisor { 1 };

    std::array<AtomicSharedSample, kMaxBankSize> sampleBank;
    int numSamplesLoaded { 0 };

    void resetCounters() noexcept { triggerCount = 0; }

    int getActiveSampleIndex() noexcept
    {
        const int n = juce::jlimit(0, kMaxBankSize, numSamplesLoaded);
        if (n <= 0)
            return -1;

        switch (selectionMode)
        {
        case SampleSelectionMode::Static:
            return juce::jlimit(0, n - 1, staticBankIndex);

        case SampleSelectionMode::ModuloCycle:
        {
            const int div = juce::jmax(1, moduloDivisor);
            return (triggerCount / div) % n;
        }

        case SampleSelectionMode::Random:
        {
            lfsr ^= lfsr >> 7;
            lfsr ^= lfsr << 9;
            lfsr ^= lfsr >> 13;
            return static_cast<int>(lfsr % static_cast<uint32_t>(n));
        }
        default:
            return 0;
        }
    }

    bool evaluateTriggerAtStep(int stepInTrack) noexcept
    {
        if (activeStepLength < 1)
            return false;

        const int s = juce::jlimit(0, activeStepLength - 1, stepInTrack);
        if (triggers[static_cast<size_t>(s)] != 0)
        {
            ++triggerCount;
            return true;
        }
        return false;
    }

    uint32_t lfsr { 0xACE1u };
};

inline void installDefaultPatterns(std::array<TrackState, kNumTracks>& tracks) noexcept
{
    for (auto& t : tracks)
    {
        t.triggers.fill(0);
        for (auto& v : t.velocities)
            v = 0.85f;
    }

    constexpr int lengths[kNumTracks] { 16, 12, 7, 5, 16, 16, 16, 16 };

    for (int ti = 0; ti < kNumTracks; ++ti)
        tracks[static_cast<size_t>(ti)].activeStepLength = lengths[ti];

    // Kick (16)
    tracks[0].triggers[0] = 1;
    tracks[0].triggers[4] = 1;
    tracks[0].triggers[8] = 1;
    tracks[0].triggers[12] = 1;

    // Snare (12) — steps placed within first 12
    tracks[1].triggers[4] = 1;
    tracks[1].triggers[10] = 1;

    // Closed hat (7)
    for (int s = 0; s < 7; s += 2)
        tracks[2].triggers[static_cast<size_t>(s)] = 1;

    // Open hat (5)
    tracks[3].triggers[2] = 1;
    tracks[3].triggers[4] = 1;
}

} // namespace huckaby
