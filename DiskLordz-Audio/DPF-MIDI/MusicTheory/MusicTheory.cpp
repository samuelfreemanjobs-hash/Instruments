#include "MusicTheory.h"

#include <algorithm>
#include <cmath>

namespace disklordz::theory
{

namespace
{

std::array<int, 7> scaleIntervals(ScaleType scale)
{
    switch (scale)
    {
    case ScaleType::naturalMinor:
        return { 0, 2, 3, 5, 7, 8, 10 };
    case ScaleType::harmonicMinor:
        return { 0, 2, 3, 5, 7, 8, 11 };
    case ScaleType::major:
        return { 0, 2, 4, 5, 7, 9, 11 };
    case ScaleType::phrygian:
        return { 0, 1, 3, 5, 7, 8, 10 };
    default:
        return { 0, 2, 3, 5, 7, 8, 10 };
    }
}

int degreeToPc(int rootPc, ScaleType scale, int degree1Based)
{
    const auto steps = scaleIntervals(scale);
    const int idx = (degree1Based - 1) % 7;
    return (rootPc + steps[static_cast<size_t>(idx)] + 12) % 12;
}

std::array<int, 4> chordSemitoneSteps(ChordQuality q)
{
    switch (q)
    {
    case ChordQuality::minor:
        return { 0, 3, 7, -1 };
    case ChordQuality::major:
        return { 0, 4, 7, -1 };
    case ChordQuality::diminished:
        return { 0, 3, 6, -1 };
    case ChordQuality::dominant7:
        return { 0, 4, 7, 10 };
    case ChordQuality::minor7:
        return { 0, 3, 7, 10 };
    case ChordQuality::major7:
        return { 0, 4, 7, 11 };
    case ChordQuality::sus2:
        return { 0, 2, 7, -1 };
    case ChordQuality::sus4:
        return { 0, 5, 7, -1 };
    default:
        return { 0, 3, 7, -1 };
    }
}

} // namespace

std::array<bool, 12> scalePitchClasses(int rootPc, ScaleType scale)
{
    std::array<bool, 12> out{};
    const auto steps = scaleIntervals(scale);
    for (int step : steps)
        out[static_cast<size_t>((rootPc + step) % 12)] = true;
    return out;
}

bool isPitchInScale(int midiNote, int rootPc, ScaleType scale)
{
    const auto pcs = scalePitchClasses(rootPc, scale);
    return pcs[static_cast<size_t>(midiNote % 12)];
}

std::vector<int> buildVoicedChord(int rootPc,
                                  ScaleType scale,
                                  int scaleDegree1Based,
                                  ChordQuality quality,
                                  int inversion,
                                  float spread01,
                                  int baseOctaveMidi)
{
    const int chordRootPc = degreeToPc(rootPc, scale, scaleDegree1Based);
    const int bassMidi = baseOctaveMidi + chordRootPc;

    std::vector<int> notes;
    const auto steps = chordSemitoneSteps(quality);
    for (int step : steps)
    {
        if (step < 0)
            continue;
        notes.push_back(bassMidi + step);
    }

    if (notes.empty())
        return notes;

    std::sort(notes.begin(), notes.end());
    inversion = std::clamp(inversion, 0, static_cast<int>(notes.size()) - 1);
    for (int i = 0; i < inversion; ++i)
    {
        const int low = notes.front();
        notes.erase(notes.begin());
        notes.push_back(low + 12);
    }

    spread01 = std::clamp(spread01, 0.0f, 1.0f);
    const int extraSpread = static_cast<int>(std::round(spread01 * 12.0));
    for (size_t i = 1; i < notes.size(); ++i)
        notes[i] += extraSpread + static_cast<int>(i) * 12;

    for (int& n : notes)
        n = std::clamp(n, 24, 96);

    return notes;
}

const char* scaleName(ScaleType scale)
{
    switch (scale)
    {
    case ScaleType::naturalMinor:
        return "Natural minor";
    case ScaleType::harmonicMinor:
        return "Harmonic minor";
    case ScaleType::major:
        return "Major";
    case ScaleType::phrygian:
        return "Phrygian";
    default:
        return "Scale";
    }
}

const char* chordQualitySymbol(ChordQuality q)
{
    switch (q)
    {
    case ChordQuality::minor:
        return "m";
    case ChordQuality::major:
        return "";
    case ChordQuality::diminished:
        return "dim";
    case ChordQuality::dominant7:
        return "7";
    case ChordQuality::minor7:
        return "m7";
    case ChordQuality::major7:
        return "maj7";
    case ChordQuality::sus2:
        return "sus2";
    case ChordQuality::sus4:
        return "sus4";
    default:
        return "?";
    }
}

const char* noteNameFromPc(int pc)
{
    static constexpr const char* names[] = { "C",  "C#", "D",  "D#", "E",  "F",
                                             "F#", "G",  "G#", "A",  "A#", "B" };
    pc = (pc % 12 + 12) % 12;
    return names[pc];
}

int chordRootPitchClass(int rootPc, ScaleType scale, int degree1Based)
{
    return degreeToPc(rootPc, scale, degree1Based);
}

std::string degreeLabel(int degree1Based, ChordQuality quality, ScaleType scale)
{
    const bool minorScale = scale == ScaleType::naturalMinor || scale == ScaleType::harmonicMinor
                            || scale == ScaleType::phrygian;
  (void)quality;
    static const char* majorNumerals[] = { "I", "II", "III", "IV", "V", "VI", "VII" };
    static const char* minorNumerals[] = { "i", "ii", "III", "iv", "v", "VI", "VII" };
    const int idx = (degree1Based - 1) % 7;
    if (minorScale)
        return minorNumerals[idx];
    return majorNumerals[idx];
}

} // namespace disklordz::theory