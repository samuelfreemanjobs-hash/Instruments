#pragma once
// Melody-Aware Drum Intelligence — key/scale/chord context + bass/percussion suggestions.
// Guidance only: never silently retunes untuned snares/hats; never deletes locked notes.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace disklordz::harmony
{

enum class ScaleType : uint8_t
{
    Major = 0,
    NaturalMinor,
    HarmonicMinor,
    Dorian,
    Mixolydian,
    PentatonicMinor,
    Blues,
    Count
};

enum class ChordQuality : uint8_t
{
    Major = 0,
    Minor,
    Dom7,
    Maj7,
    Min7,
    Sus2,
    Sus4,
    Dim,
    Count
};

struct ChordEvent
{
    int rootMidi = 60; // C4
    ChordQuality quality = ChordQuality::Minor;
    int startStep = 0;
    int lengthSteps = 4;
};

struct HarmonicContext
{
    int tonicMidi = 57; // A3 default (common trap)
    ScaleType scale = ScaleType::PentatonicMinor;
    std::array<ChordEvent, 8> progression {};
    int progressionLength = 4;
    bool enabled = true;
};

inline constexpr std::array<std::array<int, 7>, static_cast<size_t>(ScaleType::Count)> kScaleIntervals {{
    {{ 0, 2, 4, 5, 7, 9, 11 }},  // Major
    {{ 0, 2, 3, 5, 7, 8, 10 }},  // Natural minor
    {{ 0, 2, 3, 5, 7, 8, 11 }},  // Harmonic minor
    {{ 0, 2, 3, 5, 7, 9, 10 }},  // Dorian
    {{ 0, 2, 4, 5, 7, 9, 10 }},  // Mixolydian
    {{ 0, 3, 5, 7, 10, 12, 15 }}, // Pentatonic minor (extra for octave)
    {{ 0, 3, 5, 6, 7, 10, 12 }}   // Blues
}};

inline int clampMidi(int n) noexcept
{
    return std::max(0, std::min(127, n));
}

inline bool isScaleDegree(int midiNote, int tonicMidi, ScaleType scale) noexcept
{
    const int pc = ((midiNote - tonicMidi) % 12 + 12) % 12;
    const auto& iv = kScaleIntervals[static_cast<size_t>(scale) % static_cast<size_t>(ScaleType::Count)];
    for (int i = 0; i < 7; ++i)
    {
        if ((iv[static_cast<size_t>(i)] % 12) == pc)
            return true;
    }
    return false;
}

inline float midiToHz(int midiNote) noexcept
{
    return 440.0f * std::pow(2.0f, (static_cast<float>(midiNote) - 69.0f) / 12.0f);
}

inline int hzToNearestMidi(float hz) noexcept
{
    if (hz <= 1.0f)
        return 0;
    const float n = 69.0f + 12.0f * std::log2(hz / 440.0f);
    return clampMidi(static_cast<int>(std::lround(n)));
}

/** Chord tone MIDI offsets from root. */
inline void chordTones(ChordQuality q, std::array<int, 4>& out, int& count) noexcept
{
    count = 3;
    switch (q)
    {
    case ChordQuality::Major: out = {{ 0, 4, 7, 0 }}; break;
    case ChordQuality::Minor: out = {{ 0, 3, 7, 0 }}; break;
    case ChordQuality::Dom7:  out = {{ 0, 4, 7, 10 }}; count = 4; break;
    case ChordQuality::Maj7:  out = {{ 0, 4, 7, 11 }}; count = 4; break;
    case ChordQuality::Min7:  out = {{ 0, 3, 7, 10 }}; count = 4; break;
    case ChordQuality::Sus2:  out = {{ 0, 2, 7, 0 }}; break;
    case ChordQuality::Sus4:  out = {{ 0, 5, 7, 0 }}; break;
    case ChordQuality::Dim:   out = {{ 0, 3, 6, 0 }}; break;
    default: out = {{ 0, 3, 7, 0 }}; break;
    }
}

inline ChordEvent chordAtStep(const HarmonicContext& ctx, int step) noexcept
{
    if (!ctx.enabled || ctx.progressionLength <= 0)
        return ChordEvent { ctx.tonicMidi, ChordQuality::Minor, 0, 16 };

    for (int i = 0; i < ctx.progressionLength; ++i)
    {
        const auto& c = ctx.progression[static_cast<size_t>(i)];
        if (step >= c.startStep && step < c.startStep + c.lengthSteps)
            return c;
    }
    return ctx.progression[0];
}

struct NoteSuggestion
{
    int midiNote = 36;
    float velocity = 0.85f;
    int step = 0;
    bool isChordTone = true;
};

/**
 * Suggest bass MIDI notes for a 16-step bar.
 * Prefer chord roots on kick-aligned steps; optional chord tones / passing tones.
 * Does not mutate user patterns — returns suggestions only.
 */
inline void suggestBassNotes(const HarmonicContext& ctx, int bassOctaveMidi /* e.g. 36 */,
                             bool allowPassing, std::vector<NoteSuggestion>& out) noexcept
{
    out.clear();
    if (!ctx.enabled)
    {
        for (int step : { 0, 8 })
            out.push_back({ bassOctaveMidi, 0.9f, step, true });
        return;
    }

    for (int step = 0; step < 16; ++step)
    {
        // Sparse melodic-trap density: accents on 0, 3, 6, 8, 11, 14
        const bool accent = (step == 0 || step == 3 || step == 6 || step == 8 || step == 11 || step == 14);
        if (!accent)
            continue;

        const auto chord = chordAtStep(ctx, step);
        std::array<int, 4> tones {};
        int n = 0;
        chordTones(chord.quality, tones, n);

        int degree = 0;
        if (step == 3 || step == 11)
            degree = std::min(2, n - 1); // 5th-ish
        else if (step == 6 || step == 14)
            degree = (n > 1) ? 1 : 0; // 3rd
        else
            degree = 0; // root

        int note = clampMidi(bassOctaveMidi + (chord.rootMidi % 12) + tones[static_cast<size_t>(degree)]);
        // Snap into requested octave register
        while (note > bassOctaveMidi + 11)
            note -= 12;
        while (note < bassOctaveMidi - 5)
            note += 12;

        if (allowPassing && (step == 7 || step == 15))
        {
            // Approach from below by scale step
            note = clampMidi(note - 1);
            if (!isScaleDegree(note, ctx.tonicMidi, ctx.scale))
                note = clampMidi(note - 1);
        }

        NoteSuggestion s;
        s.midiNote = note;
        s.velocity = (step == 0 || step == 8) ? 0.92f : 0.75f;
        s.step = step;
        s.isChordTone = true;
        out.push_back(s);
    }
}

/** Suggest chromatic MIDI for Keys & Bells accents on offbeats (chord tones only). */
inline void suggestBellAccents(const HarmonicContext& ctx, int bellOctaveMidi /* e.g. 72 */,
                               std::vector<NoteSuggestion>& out) noexcept
{
    out.clear();
    for (int step : { 2, 6, 10, 14 })
    {
        const auto chord = chordAtStep(ctx, step);
        std::array<int, 4> tones {};
        int n = 0;
        chordTones(chord.quality, tones, n);
        const int deg = (step == 6 || step == 14) ? std::min(2, n - 1) : 0;
        NoteSuggestion s;
        s.midiNote = clampMidi(bellOctaveMidi + (chord.rootMidi % 12) + tones[static_cast<size_t>(deg)]);
        s.velocity = 0.55f;
        s.step = step;
        s.isChordTone = true;
        out.push_back(s);
    }
}

/** Default i–VI–III–VII style minor progression in tonic key (4 bars / 16 steps). */
inline HarmonicContext makeDefaultTrapProgression(int tonicMidi = 57) noexcept
{
    HarmonicContext ctx;
    ctx.tonicMidi = tonicMidi;
    ctx.scale = ScaleType::PentatonicMinor;
    ctx.progressionLength = 4;
    // i
    ctx.progression[0] = { tonicMidi, ChordQuality::Minor, 0, 4 };
    // VI
    ctx.progression[1] = { tonicMidi + 8, ChordQuality::Major, 4, 4 };
    // III
    ctx.progression[2] = { tonicMidi + 3, ChordQuality::Major, 8, 4 };
    // VII
    ctx.progression[3] = { tonicMidi + 10, ChordQuality::Major, 12, 4 };
    return ctx;
}

} // namespace disklordz::harmony
