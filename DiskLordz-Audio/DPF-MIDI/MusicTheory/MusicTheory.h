#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace disklordz::theory
{

enum class ScaleType : std::uint8_t
{
    naturalMinor = 0,
    harmonicMinor,
    major,
    phrygian,
    count
};

enum class ChordQuality : std::uint8_t
{
    minor = 0,
    major,
    diminished,
    dominant7,
    minor7,
    major7,
    sus2,
    sus4,
    count
};

constexpr int numScaleTypes = static_cast<int>(ScaleType::count);
constexpr int numKeys = 12;

/** Pitch-class set for a scale rooted at `rootPc` (0 = C). */
std::array<bool, 12> scalePitchClasses(int rootPc, ScaleType scale);

bool isPitchInScale(int midiNote, int rootPc, ScaleType scale);

/** Returns MIDI note numbers (may span multiple octaves) for one chord voicing. */
std::vector<int> buildVoicedChord(int rootPc,
                                  ScaleType scale,
                                  int scaleDegree1Based,
                                  ChordQuality quality,
                                  int inversion,
                                  float spread01,
                                  int baseOctaveMidi = 48);

const char* scaleName(ScaleType scale);
const char* chordQualitySymbol(ChordQuality q);
const char* noteNameFromPc(int pc);

/** Roman / degree label for UI, e.g. "i", "VI". */
std::string degreeLabel(int degree1Based, ChordQuality quality, ScaleType scale);

int chordRootPitchClass(int rootPc, ScaleType scale, int degree1Based);

} // namespace disklordz::theory