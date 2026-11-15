#pragma once
// BANG Rhythmic Oddity Engine + Minimal Monster pattern reduction.
#include "../DSP/CoreMathAndPRNG.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

namespace disklordz::bang
{

enum class GrooveFamily : uint8_t
{
    MilliPressure = 0,
    DirtyDigital,
    BossBounce,
    SouthernOddity,
    RawMachine,
    MinimalMonster,
    EccentricAtlanta,
    MechanicalBounce,
    DigitalFunk,
    ExperimentalStreet,
    SparseKnock,
    RhythmicHypnosis,
    Count
};

enum class MinimalMode : uint8_t
{
    BareBones = 0,
    Minimal,
    Balanced,
    Aggressive,
    Experimental,
    Count
};

struct StepHit
{
    uint8_t pad = 0;
    float   velocity = 0.85f;
    float   microMs = 0.f;
    bool    active = false;
    bool    locked = false;
};

struct GrooveTrack
{
    std::array<StepHit, 64> steps {};
    float swing01 = 0.1f;
};

struct OddityPattern
{
    GrooveFamily family = GrooveFamily::MilliPressure;
    int   steps = 16;
    float bpm = 92.f;
    GrooveTrack kick;
    GrooveTrack snare;
    GrooveTrack hat;
    GrooveTrack perc;
};

inline float swingMs(float swing01, int step, float stepMs) noexcept
{
    return ((step & 1) != 0) ? swing01 * stepMs * 0.4f : 0.f;
}

/** Motif-based grooves: repetition + selective silence + accent displacement. */
inline OddityPattern makeOddity(GrooveFamily family, uint64_t seed = 1ull) noexcept
{
    OddityPattern g;
    g.family = family;
    dsp::DeterministicRng rng(seed ^ (static_cast<uint64_t>(family) * 0x9E3779B97F4A7C15ULL));

    auto clear = [](GrooveTrack& t) {
        for (auto& s : t.steps)
            s = StepHit {};
    };
    clear(g.kick);
    clear(g.snare);
    clear(g.hat);
    clear(g.perc);

    auto placeKick = [&](std::initializer_list<int> idxs, float vel, float anticMs) {
        for (int i : idxs)
        {
            if (i < 0 || i >= 16)
                continue;
            auto& s = g.kick.steps[static_cast<size_t>(i)];
            s.pad = 0;
            s.active = true;
            s.velocity = vel;
            s.microMs = -anticMs;
        }
    };
    auto placeSnare = [&](std::initializer_list<int> idxs, float vel, float delayMs) {
        for (int i : idxs)
        {
            if (i < 0 || i >= 16)
                continue;
            auto& s = g.snare.steps[static_cast<size_t>(i)];
            s.pad = 2;
            s.active = true;
            s.velocity = vel;
            s.microMs = delayMs;
        }
    };
    auto fillHatsSparse = [&](float dens, float swing, float vel) {
        g.hat.swing01 = swing;
        for (int i = 0; i < 16; ++i)
        {
            auto& s = g.hat.steps[static_cast<size_t>(i)];
            s.pad = 6;
            // Motif: accents on 0,3,6,8,11 — silence elsewhere unless density high
            const bool motif = (i == 0 || i == 3 || i == 6 || i == 8 || i == 11 || i == 14);
            s.active = motif || (dens > 0.7f && (i % 2) == 0);
            if (dens < 0.35f && (i == 3 || i == 11))
                s.active = false;
            s.velocity = vel * (motif ? 0.9f : 0.55f);
        }
    };
    auto placePerc = [&](std::initializer_list<int> idxs) {
        for (int i : idxs)
        {
            if (i < 0 || i >= 16)
                continue;
            auto& s = g.perc.steps[static_cast<size_t>(i)];
            s.pad = 5;
            s.active = true;
            s.velocity = 0.55f + 0.25f * rng.nextUnit();
            s.microMs = 2.f + rng.nextSigned();
        }
    };

    switch (family)
    {
    case GrooveFamily::MilliPressure:
        g.bpm = 94.f;
        placeKick({ 0, 3, 6, 8, 11 }, 0.92f, 3.f); // relentless off-grid push
        placeSnare({ 4, 12 }, 0.9f, 1.f);
        fillHatsSparse(0.55f, 0.08f, 0.7f);
        placePerc({ 2, 7, 10, 14 });
        break;
    case GrooveFamily::DirtyDigital:
        g.bpm = 96.f;
        placeKick({ 0, 7, 8 }, 0.95f, 2.f);
        placeSnare({ 4, 11, 12 }, 0.88f, 0.f);
        fillHatsSparse(0.65f, 0.12f, 0.8f);
        placePerc({ 1, 5, 9, 13 });
        break;
    case GrooveFamily::BossBounce:
        g.bpm = 90.f;
        placeKick({ 0, 8 }, 0.96f, 4.f);
        placeSnare({ 4, 12 }, 0.92f, 2.f);
        fillHatsSparse(0.4f, 0.1f, 0.65f);
        placePerc({ 6, 14 });
        break;
    case GrooveFamily::SouthernOddity:
        g.bpm = 88.f;
        placeKick({ 0, 5, 8, 13 }, 0.9f, 5.f);
        placeSnare({ 4, 10 }, 0.85f, 3.f);
        fillHatsSparse(0.45f, 0.18f, 0.6f);
        placePerc({ 2, 7, 11, 15 });
        break;
    case GrooveFamily::RawMachine:
        g.bpm = 98.f;
        placeKick({ 0, 4, 8, 12 }, 0.94f, 1.f);
        placeSnare({ 4, 12 }, 0.9f, 0.f);
        fillHatsSparse(0.75f, 0.05f, 0.85f);
        placePerc({ 3, 7, 11, 15 });
        break;
    case GrooveFamily::MinimalMonster:
        g.bpm = 86.f;
        placeKick({ 0 }, 1.0f, 2.f);
        placeSnare({ 8 }, 0.95f, 4.f);
        fillHatsSparse(0.2f, 0.06f, 0.5f);
        // silence most hats
        for (int i = 0; i < 16; ++i)
            if (i != 0 && i != 8)
                g.hat.steps[static_cast<size_t>(i)].active = false;
        break;
    case GrooveFamily::EccentricAtlanta:
        g.bpm = 92.f;
        placeKick({ 0, 6, 8, 14 }, 0.88f, 6.f);
        placeSnare({ 4, 11 }, 0.82f, 2.f);
        fillHatsSparse(0.5f, 0.2f, 0.7f);
        placePerc({ 1, 3, 9, 12 });
        break;
    case GrooveFamily::MechanicalBounce:
        g.bpm = 100.f;
        placeKick({ 0, 3, 8, 11 }, 0.9f, 0.f);
        placeSnare({ 4, 12 }, 0.88f, 0.f);
        fillHatsSparse(0.7f, 0.0f, 0.85f);
        placePerc({ 2, 6, 10, 14 });
        break;
    case GrooveFamily::DigitalFunk:
        g.bpm = 102.f;
        placeKick({ 0, 7, 10 }, 0.9f, 3.f);
        placeSnare({ 4, 12, 15 }, 0.85f, 1.f);
        fillHatsSparse(0.8f, 0.22f, 0.88f);
        placePerc({ 1, 5, 9, 13 });
        break;
    case GrooveFamily::ExperimentalStreet:
        g.bpm = 91.f;
        placeKick({ 0, 2, 8, 9 }, 0.93f, 4.f);
        placeSnare({ 5, 12 }, 0.87f, 2.f);
        fillHatsSparse(0.35f, 0.15f, 0.6f);
        placePerc({ 3, 6, 11, 14 });
        break;
    case GrooveFamily::SparseKnock:
        g.bpm = 84.f;
        placeKick({ 0, 10 }, 0.97f, 3.f);
        placeSnare({ 8 }, 0.9f, 5.f);
        fillHatsSparse(0.25f, 0.08f, 0.45f);
        placePerc({ 6 });
        break;
    case GrooveFamily::RhythmicHypnosis:
        g.bpm = 93.f;
        placeKick({ 0, 3, 6, 9, 12, 15 }, 0.86f, 2.f); // ostinato
        placeSnare({ 4, 12 }, 0.8f, 1.f);
        fillHatsSparse(0.6f, 0.1f, 0.72f);
        placePerc({ 1, 7, 13 });
        break;
    default:
        placeKick({ 0, 8 }, 0.9f, 3.f);
        placeSnare({ 4, 12 }, 0.85f, 2.f);
        fillHatsSparse(0.5f, 0.1f, 0.7f);
        break;
    }
    return g;
}

inline OddityPattern applyMinimalMonster(OddityPattern g, MinimalMode mode) noexcept
{
    if (mode == MinimalMode::Aggressive && g.family == GrooveFamily::MinimalMonster)
        return g; // already sparse

    const bool keepGhostSnare = (mode == MinimalMode::Balanced || mode == MinimalMode::Aggressive
                                 || mode == MinimalMode::Experimental);
    const bool keepPerc = (mode == MinimalMode::Balanced || mode == MinimalMode::Aggressive
                           || mode == MinimalMode::Experimental);
    const bool keepOffbeatKick = (mode == MinimalMode::Aggressive || mode == MinimalMode::Experimental);

    for (int i = 0; i < g.steps; ++i)
    {
        auto& k = g.kick.steps[static_cast<size_t>(i)];
        auto& s = g.snare.steps[static_cast<size_t>(i)];
        auto& h = g.hat.steps[static_cast<size_t>(i)];
        auto& p = g.perc.steps[static_cast<size_t>(i)];

        if (!k.locked && mode == MinimalMode::BareBones && i != 0 && i != 8)
            k.active = false;
        if (!k.locked && mode == MinimalMode::Minimal && !keepOffbeatKick && i != 0 && i != 8 && i != 4)
            k.active = false;
        if (!s.locked && mode == MinimalMode::BareBones && i != 4 && i != 8 && i != 12)
            s.active = false;
        if (!s.locked && !keepGhostSnare && s.velocity < 0.6f)
            s.active = false;
        if (!h.locked)
        {
            if (mode == MinimalMode::BareBones)
                h.active = (i == 0 || i == 8);
            else if (mode == MinimalMode::Minimal && (i % 2) == 1)
                h.active = false;
        }
        if (!p.locked && !keepPerc)
            p.active = false;
        if (!p.locked && mode == MinimalMode::Experimental && rngSkip(i))
            p.active = !p.active; // invert sparse perc for experimental
    }
    return g;
}

inline bool rngSkip(int i) noexcept
{
    // Deterministic alternate without PRNG dependency in reduction path
    return (i % 3) == 0;
}

struct ScheduledHit
{
    int sampleOffset = 0;
    StepHit hit {};
};

inline void expandOddity(const OddityPattern& g, double sampleRate, std::vector<ScheduledHit>& out)
{
    out.clear();
    const double stepMs = (60000.0 / static_cast<double>(g.bpm)) / 4.0;
    auto collect = [&](const GrooveTrack& t) {
        for (int i = 0; i < g.steps; ++i)
        {
            const auto& s = t.steps[static_cast<size_t>(i)];
            if (!s.active)
                continue;
            const float sw = swingMs(t.swing01, i, static_cast<float>(stepMs));
            const double tMs = static_cast<double>(i) * stepMs + static_cast<double>(s.microMs + sw);
            ScheduledHit sh;
            sh.sampleOffset = static_cast<int>(std::max(0.0, tMs * 0.001 * sampleRate));
            sh.hit = s;
            out.push_back(sh);
        }
    };
    collect(g.kick);
    collect(g.snare);
    collect(g.hat);
    collect(g.perc);
}

} // namespace disklordz::bang
