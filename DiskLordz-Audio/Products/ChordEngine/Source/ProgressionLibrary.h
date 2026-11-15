#pragma once

#include "../../../DPF-MIDI/MusicTheory/MusicTheory.h"

#include <array>
#include <cstddef>

namespace disklordz::chordengine
{

struct ChordSlot
{
    int degree1Based;
    disklordz::theory::ChordQuality quality;
};

struct StarterProgression
{
    const char* id;
    const char* displayName;
    const char* genreTag;
    disklordz::theory::ScaleType recommendedScale;
    int defaultRootPc;
    std::array<ChordSlot, 4> slots;
};

constexpr int kNumStarterProgressions = 6;

const std::array<StarterProgression, kNumStarterProgressions>& starterProgressions();

} // namespace disklordz::chordengine