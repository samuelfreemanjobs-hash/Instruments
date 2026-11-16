#pragma once

#include "../ThreeLayerDrum.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace disklordz::dna
{

// Supports all 4 DiskLordz DNA matrices on one mathematically unified 2D engine:
// 0 = HyperDNA (Plugg, Rage, Hypertrap, Hyperpop, Digicore)
// 1 = PercussionDNA (BRICK, WARFARE, ANTHEM, DARK, ARENA)
// 2 = ZayDNA (BrickTrap, TrapGospel, Classic2000s, BeastBounce, LuxuryAtlanta)
// 3 = SouthDNA (MafiaPressure, DarkFuture, StreetWarfare, SavageTrap, Nightmare)
enum class MatrixMode : uint8_t
{
    HyperDNA = 0,
    PercussionDNA,
    ZayDNA,
    SouthDNA
};

struct LockMask
{
    bool lockRoot      = true;
    bool lockPitchEnv  = false;
    bool lockTransient = false;
    bool lockDecay     = false;
    bool lockDrive     = false;
    bool lockTexture   = false;
};

struct NodeTarget
{
    const char* name          = "";
    float       x             = 0.5f;
    float       y             = 0.5f;
    float       rootHz        = 50.0f;
    float       pitchStartHz  = 155.0f;
    float       pitchMs       = 35.0f;
    float       decayMs       = 450.0f;
    float       glideMs       = 25.0f;
    float       drive         = 2.0f;
    float       fmIndex       = 0.5f;
    float       noiseMix      = 0.25f;
    float       samplerGrit   = 0.0f;
    float       transientLvl  = 0.50f;
    float       reverbMix     = 0.08f;
};

class HyperDNAMorph
{
public:
    static constexpr size_t kNodeCount = 5;
    using Weights                      = std::array<float, kNodeCount>;

    static const std::array<NodeTarget, kNodeCount>& nodesFor(MatrixMode mode) noexcept
    {
        static const std::array<NodeTarget, kNodeCount> kHyper {{
            { "Plugg",     0.12f, 0.16f, 50.f, 120.f, 55.f, 520.f, 20.f, 1.15f, 0.15f, 0.12f, 0.00f, 0.30f, 0.08f },
            { "Rage",      0.85f, 0.14f, 50.f, 195.f, 24.f, 400.f, 30.f, 4.40f, 0.35f, 0.28f, 0.04f, 0.90f, 0.05f },
            { "Hypertrap", 0.77f, 0.68f, 50.f, 180.f, 32.f, 620.f, 90.f, 3.40f, 0.90f, 0.30f, 0.12f, 0.78f, 0.10f },
            { "Hyperpop",  0.22f, 0.85f, 50.f, 145.f, 62.f, 330.f, 65.f, 2.40f, 2.40f, 0.48f, 0.35f, 0.68f, 0.14f },
            { "Digicore",  0.51f, 0.47f, 50.f, 170.f, 44.f, 420.f, 42.f, 3.70f, 1.85f, 0.55f, 0.78f, 0.82f, 0.09f }
        }};
        static const std::array<NodeTarget, kNodeCount> kPerc {{
            { "BRICK",   0.14f, 0.18f, 55.f, 175.f, 18.f, 260.f, 5.f,  2.8f, 0.2f, 0.22f, 0.10f, 0.85f, 0.06f },
            { "WARFARE", 0.86f, 0.18f, 55.f, 210.f, 22.f, 780.f, 12.f, 4.2f, 0.8f, 0.40f, 0.08f, 0.92f, 0.20f },
            { "ANTHEM",  0.80f, 0.82f, 55.f, 160.f, 34.f, 950.f, 15.f, 2.2f, 0.5f, 0.30f, 0.00f, 0.74f, 0.26f },
            { "DARK",    0.18f, 0.82f, 55.f, 125.f, 48.f, 880.f, 20.f, 1.9f, 1.1f, 0.18f, 0.18f, 0.45f, 0.22f },
            { "ARENA",   0.50f, 0.50f, 55.f, 170.f, 28.f, 1200.f,10.f, 2.6f, 0.6f, 0.35f, 0.00f, 0.80f, 0.32f }
        }};
        static const std::array<NodeTarget, kNodeCount> kZay {{
            { "BrickTrap",     0.15f, 0.20f, 49.f, 168.f, 26.f, 420.f, 15.f, 3.1f, 0.2f, 0.24f, 0.12f, 0.82f, 0.04f },
            { "TrapGospel",    0.84f, 0.20f, 49.f, 132.f, 38.f, 540.f, 28.f, 1.5f, 0.9f, 0.18f, 0.00f, 0.58f, 0.14f },
            { "Classic2000s",  0.80f, 0.80f, 49.f, 155.f, 22.f, 310.f, 8.f,  2.2f, 0.4f, 0.26f, 0.55f, 0.76f, 0.05f },
            { "BeastBounce",   0.20f, 0.82f, 49.f, 190.f, 20.f, 490.f, 18.f, 3.6f, 0.3f, 0.22f, 0.08f, 0.88f, 0.06f },
            { "LuxuryAtlanta", 0.50f, 0.50f, 49.f, 140.f, 34.f, 580.f, 32.f, 1.7f, 0.7f, 0.16f, 0.00f, 0.64f, 0.12f }
        }};
        static const std::array<NodeTarget, kNodeCount> kSouth {{
            { "MafiaPressure", 0.14f, 0.16f, 46.f, 195.f, 20.f, 460.f, 20.f, 4.1f, 0.2f, 0.24f, 0.05f, 0.88f, 0.04f },
            { "DarkFuture",    0.86f, 0.16f, 46.f, 150.f, 36.f, 680.f, 45.f, 2.9f, 0.8f, 0.18f, 0.10f, 0.65f, 0.16f },
            { "StreetWarfare", 0.82f, 0.82f, 46.f, 215.f, 18.f, 390.f, 15.f, 4.8f, 0.5f, 0.34f, 0.15f, 0.94f, 0.06f },
            { "SavageTrap",    0.18f, 0.82f, 46.f, 180.f, 24.f, 510.f, 55.f, 3.5f, 0.4f, 0.22f, 0.00f, 0.84f, 0.07f },
            { "Blackout",      0.50f, 0.50f, 46.f, 225.f, 16.f, 430.f, 35.f, 5.6f, 1.4f, 0.42f, 0.28f, 0.96f, 0.08f }
        }};

        switch (mode)
        {
            case MatrixMode::HyperDNA:      return kHyper;
            case MatrixMode::PercussionDNA: return kPerc;
            case MatrixMode::ZayDNA:        return kZay;
            case MatrixMode::SouthDNA:      return kSouth;
        }
        return kHyper;
    }

    static Weights calculateWeights(float x, float y, MatrixMode mode = MatrixMode::HyperDNA) noexcept
    {
        const float cx    = std::clamp(x, 0.0f, 1.0f);
        const float cy    = std::clamp(y, 0.0f, 1.0f);
        const auto& nodes = nodesFor(mode);

        Weights result {};
        float   total = 0.0f;
        for (size_t i = 0; i < kNodeCount; ++i)
        {
            const float dx   = cx - nodes[i].x;
            const float dy   = cy - nodes[i].y;
            const float dist = std::sqrt(dx * dx + dy * dy);
            result[i]        = 1.0f / std::pow(dist + 0.08f, 2.6f);
            total += result[i];
        }
        for (auto& w : result)
            w /= std::max(total, 1e-9f);
        return result;
    }

    // Single-pass corrected log/linear recipe morph preserving instrument invariants & locks.
    static DrumRecipe morphRecipe(
        const DrumRecipe& source,
        const Weights&    weights,
        float             intensity,
        LockMask          locks = {},
        MatrixMode        mode  = MatrixMode::HyperDNA) noexcept
    {
        const float t = std::clamp(intensity, 0.0f, 1.0f);
        if (t <= 0.0f)
            return source;

        const auto& nodes = nodesFor(mode);
        DrumRecipe  out   = source;

        auto logBlend = [&](float srcVal, auto getter) noexcept -> float {
            float targetLog = 0.0f;
            for (size_t i = 0; i < kNodeCount; ++i)
                targetLog += weights[i] * std::log(std::max(getter(nodes[i]), 0.001f));
            const float origLog = std::log(std::max(srcVal, 0.001f));
            return std::exp(origLog + (targetLog - origLog) * t);
        };

        auto linBlend = [&](float srcVal, auto getter) noexcept -> float {
            float target = 0.0f;
            for (size_t i = 0; i < kNodeCount; ++i)
                target += weights[i] * getter(nodes[i]);
            return srcVal + (target - srcVal) * t;
        };

        const float preservedRoot = locks.lockRoot ? source.body.tuneHz
                                                   : logBlend(source.body.tuneHz, [](const NodeTarget& n) { return n.rootHz; });
        out.rootHz      = locks.lockRoot ? source.rootHz : preservedRoot;
        out.body.tuneHz = preservedRoot;

        // Corrected §6 single-pass relative pitch-start interpolation (intensity applied ONCE!)
        if (!locks.lockPitchEnv)
        {
            float logTargetRatio = 0.0f;
            for (size_t i = 0; i < kNodeCount; ++i)
            {
                const float ratio = std::max(nodes[i].pitchStartHz / std::max(nodes[i].rootHz, 1.0f), 1.0f);
                logTargetRatio += weights[i] * std::log(ratio);
            }
            const float origRatio = std::max(source.body.pitchStart / std::max(source.body.tuneHz, 1.0f), 1.0f);
            out.body.pitchStart   = preservedRoot * std::exp(std::log(origRatio) + (logTargetRatio - std::log(origRatio)) * t);
            out.body.pitchMs      = logBlend(source.body.pitchMs, [](const NodeTarget& n) { return n.pitchMs; });
            out.glideMs           = logBlend(std::max(source.glideMs, 1.0f), [](const NodeTarget& n) { return n.glideMs; });
            if (source.glideMs <= 0.5f && t < 0.15f)
                out.glideMs = 0.0f;
        }

        if (!locks.lockDecay && !source.body.locked)
        {
            float newDecay = logBlend(source.body.decayMs, [](const NodeTarget& n) { return n.decayMs; });
            // Instrument-specific invariants: closed hats & kicks never turn into 1.5 s drones
            if (source.family == InstrumentFamily::ClosedHat)
                newDecay = std::min(newDecay, 95.0f);
            else if (source.family == InstrumentFamily::Kick)
                newDecay = std::min(newDecay, 280.0f);
            out.body.decayMs = newDecay;
        }

        if (!locks.lockDrive)
            out.drive = logBlend(source.drive, [](const NodeTarget& n) { return n.drive; });

        if (!locks.lockTransient && !source.attack.locked)
            out.attack.level = std::clamp(linBlend(source.attack.level, [](const NodeTarget& n) { return n.transientLvl; }), 0.0f, 1.2f);

        if (!locks.lockTexture && !source.texture.locked)
        {
            out.body.fmIndex       = std::clamp(linBlend(source.body.fmIndex, [](const NodeTarget& n) { return n.fmIndex; }), 0.0f, 6.0f);
            out.texture.level      = std::clamp(linBlend(source.texture.level, [](const NodeTarget& n) { return n.noiseMix * 1.5f; }), 0.0f, 1.2f);
            out.samplerGrit12Bit   = std::clamp(linBlend(source.samplerGrit12Bit, [](const NodeTarget& n) { return n.samplerGrit; }), 0.0f, 1.0f);
            out.reverbMix          = std::clamp(linBlend(source.reverbMix, [](const NodeTarget& n) { return n.reverbMix; }), 0.0f, 0.45f);
        }

        return out;
    }
};

} // namespace disklordz::dna
