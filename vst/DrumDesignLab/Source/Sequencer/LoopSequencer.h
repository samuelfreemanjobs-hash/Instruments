#pragma once
// Editable 16-step × 4-track loop + deterministic loop generator.
#include "RhythmicOddity.h"
#include <array>
#include <cstdint>

namespace disklordz::loop_seq
{

enum class Track : uint8_t { Kick = 0, Snare, Hat, Perc, Count };

struct Cell
{
    bool active = false;
    float velocity = 0.85f;
};

struct LoopPattern
{
    static constexpr int kSteps = 16;
    static constexpr int kTracks = 4;
    std::array<std::array<Cell, kSteps>, kTracks> grid {};
    float bpm = 140.f;
    float swing01 = 0.12f;
};

inline void clear(LoopPattern& p) noexcept
{
    for (auto& tr : p.grid)
        for (auto& c : tr)
            c = Cell {};
}

inline void fromOddity(LoopPattern& out, const bang::OddityPattern& g) noexcept
{
    clear(out);
    out.bpm = g.bpm;
    out.swing01 = g.hat.swing01;
    auto copy = [&](const bang::GrooveTrack& src, Track t) {
        for (int i = 0; i < LoopPattern::kSteps; ++i)
        {
            const auto& s = src.steps[static_cast<size_t>(i)];
            out.grid[static_cast<size_t>(t)][static_cast<size_t>(i)] = { s.active, s.velocity };
        }
    };
    copy(g.kick, Track::Kick);
    copy(g.snare, Track::Snare);
    copy(g.hat, Track::Hat);
    copy(g.perc, Track::Perc);
}

/** Deterministic trap/phonk-ish generator. */
inline LoopPattern generate(uint64_t seed, float bpm = 140.f, float density = 0.55f) noexcept
{
    LoopPattern p;
    clear(p);
    p.bpm = bpm;
    p.swing01 = 0.08f + 0.2f * static_cast<float>((seed >> 8) & 0xff) / 255.f;

    auto u = [&](int k) {
        uint64_t x = seed + static_cast<uint64_t>(k) * 0x9E3779B97F4A7C15ULL;
        x ^= x >> 33;
        x *= 0xff51afd7ed558ccdULL;
        return static_cast<float>((x >> 40) & 0xffff) / 65535.f;
    };

    // Kick: 1 and often 3, plus occasional offbeats
    p.grid[0][0] = { true, 0.95f };
    p.grid[0][8] = { true, 0.92f };
    if (u(1) > 0.45f)
        p.grid[0][4] = { true, 0.7f };
    if (u(2) > 0.55f)
        p.grid[0][10] = { true, 0.75f };
    if (density > 0.6f && u(3) > 0.4f)
        p.grid[0][6] = { true, 0.65f };

    // Snare: 2 and 4
    p.grid[1][4] = { true, 0.9f };
    p.grid[1][12] = { true, 0.88f };
    if (u(4) > 0.7f)
        p.grid[1][11] = { true, 0.55f };

    // Hats: 8ths / 16ths by density
    for (int i = 0; i < 16; ++i)
    {
        const bool eighth = (i % 2) == 0;
        const bool on = density > 0.7f ? true : (eighth || u(10 + i) < density * 0.35f);
        if (on)
            p.grid[2][static_cast<size_t>(i)] = { true, eighth ? 0.72f : 0.45f };
    }

    // Perc sparse
    for (int i : { 2, 7, 9, 14 })
        if (u(30 + i) < density)
            p.grid[3][static_cast<size_t>(i)] = { true, 0.55f + 0.3f * u(40 + i) };

    return p;
}

/** Pad indices used by the UI grid for each track. */
inline int trackPad(Track t) noexcept
{
    switch (t)
    {
    case Track::Kick: return 0;
    case Track::Snare: return 2;
    case Track::Hat: return 6;
    case Track::Perc: return 5;
    default: return 0;
    }
}

} // namespace disklordz::loop_seq
