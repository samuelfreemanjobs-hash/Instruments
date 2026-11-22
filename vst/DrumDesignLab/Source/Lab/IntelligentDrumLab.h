#pragma once
// DiskLordz Intelligent Drum Laboratory — prompt → deterministic DSP → QA → ranked export.
// Core path never requires generative AI; optional texture models stay off-RT / offline.
#include "../Export/WavExporter.h"
#include "../Presets/HitmakerBank.h"
#include "../ThreeLayerDrum.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace disklordz::lab
{

enum class LabInstrument : uint8_t
{
    Kick = 0,
    Bass808,
    Snare,
    Count
};

enum class LabCharacter : uint8_t
{
    Neutral = 0,
    Hard,
    Distorted,
    Memphis,
    Dark,
    Bright,
    Minimal,
    Punchy
};

struct PromptSpec
{
    LabInstrument instrument = LabInstrument::Kick;
    int count = 8;
    float rootHz = 65.41f; // C2 default
    char rootName[8] = "C2";
    bool mpcOptimized = false; // slightly shorter tails, mono-leaning
    bool shortTails = false;
    bool gritMid = false;
    LabCharacter characters[4] {};
    int characterCount = 0;
    uint64_t seed = 0x1D15C1ABULL;
    char rawPrompt[256] {};
};

struct RankedShot
{
    DrumRecipe recipe {};
    export_wav::QaReport qa {};
    float score = 0.f; // higher = better match
    char filename[96] {};
    std::vector<float> interleaved; // LRLR
    int frames = 0;
};

inline bool containsCi(const char* hay, const char* needle) noexcept
{
    if (hay == nullptr || needle == nullptr || needle[0] == '\0')
        return false;
    const size_t n = std::strlen(needle);
    for (const char* p = hay; *p; ++p)
    {
        size_t i = 0;
        while (i < n)
        {
            const char a = static_cast<char>(std::tolower(static_cast<unsigned char>(p[i])));
            const char b = static_cast<char>(std::tolower(static_cast<unsigned char>(needle[i])));
            if (a != b)
                break;
            ++i;
        }
        if (i == n)
            return true;
    }
    return false;
}

inline void addCharacter(PromptSpec& s, LabCharacter c) noexcept
{
    if (s.characterCount >= 4)
        return;
    for (int i = 0; i < s.characterCount; ++i)
        if (s.characters[i] == c)
            return;
    s.characters[s.characterCount++] = c;
}

/** Keyword heuristic parser (no network / no LLM). Replace later with LLM→JSON if desired. */
inline PromptSpec parsePrompt(const char* text, uint64_t seed = 0x1D15C1ABULL) noexcept
{
    PromptSpec s {};
    s.seed = seed;
    if (text != nullptr)
    {
        std::memset(s.rawPrompt, 0, sizeof(s.rawPrompt));
        std::memcpy(s.rawPrompt, text, std::min(std::strlen(text), sizeof(s.rawPrompt) - 1));
    }

    if (containsCi(text, "808") || containsCi(text, "sub"))
        s.instrument = LabInstrument::Bass808;
    else if (containsCi(text, "snare"))
        s.instrument = LabInstrument::Snare;
    else
        s.instrument = LabInstrument::Kick;

    // Count: first integer 1..128 in prompt
    if (text != nullptr)
    {
        for (const char* p = text; *p; ++p)
        {
            if (*p >= '1' && *p <= '9')
            {
                int v = 0;
                while (*p >= '0' && *p <= '9' && v < 1000)
                {
                    v = v * 10 + (*p - '0');
                    ++p;
                }
                if (v >= 1 && v <= 128)
                    s.count = v;
                break;
            }
        }
    }

    // Tuning shortcuts
    if (containsCi(text, "tuned to c") || containsCi(text, "tune to c") || containsCi(text, " in c")
        || containsCi(text, " to c,"))
    {
        s.rootHz = 65.4064f; // C2
        std::memcpy(s.rootName, "C2", 3);
        s.rootName[3] = '\0';
    }
    if (containsCi(text, "c1"))
    {
        s.rootHz = 32.7032f;
        std::memcpy(s.rootName, "C1", 3);
        s.rootName[3] = '\0';
    }
    if (containsCi(text, "g1"))
    {
        s.rootHz = 48.9994f;
        std::memcpy(s.rootName, "G1", 3);
        s.rootName[3] = '\0';
    }

    if (containsCi(text, "mpc"))
        s.mpcOptimized = true;
    if (containsCi(text, "short"))
        s.shortTails = true;
    if (containsCi(text, "grit") || containsCi(text, "midrange") || containsCi(text, "mid-range"))
        s.gritMid = true;

    if (containsCi(text, "hard"))
        addCharacter(s, LabCharacter::Hard);
    if (containsCi(text, "distort"))
        addCharacter(s, LabCharacter::Distorted);
    if (containsCi(text, "memphis"))
        addCharacter(s, LabCharacter::Memphis);
    if (containsCi(text, "dark"))
        addCharacter(s, LabCharacter::Dark);
    if (containsCi(text, "bright"))
        addCharacter(s, LabCharacter::Bright);
    if (containsCi(text, "minimal"))
        addCharacter(s, LabCharacter::Minimal);
    if (containsCi(text, "punch"))
        addCharacter(s, LabCharacter::Punchy);

    return s;
}

inline DrumRecipe baseRecipeFor(const PromptSpec& spec) noexcept
{
    DrumRecipe r {};
    switch (spec.instrument)
    {
    case LabInstrument::Bass808:
        r = makeFactoryRecipe(InstrumentFamily::Bass808, cinematic::Mood::Aggressive, spec.seed);
        break;
    case LabInstrument::Snare:
        r = makeFactoryRecipe(InstrumentFamily::Snare, cinematic::Mood::Aggressive, spec.seed);
        break;
    case LabInstrument::Kick:
    default:
        r = hitmaker::makeKick(hitmaker::Bank::Earthquake, spec.seed);
        break;
    }
    r.rootHz = spec.rootHz;
    r.body.tuneHz = spec.rootHz;
    r.pitched = (spec.instrument != LabInstrument::Snare);
    return r;
}

inline void applyCharacters(DrumRecipe& r, const PromptSpec& spec, uint64_t variant) noexcept
{
    auto has = [&](LabCharacter c) {
        for (int i = 0; i < spec.characterCount; ++i)
            if (spec.characters[i] == c)
                return true;
        return false;
    };

    const float jitter = static_cast<float>((variant % 97u) + 1u) / 100.0f;
    const float spread = static_cast<float>(variant) * 0.07f;

    if (has(LabCharacter::Hard) || has(LabCharacter::Punchy))
    {
        r.drive = std::min(7.0f, r.drive + 0.5f + jitter * 0.9f + spread * 0.15f);
        r.attack.decayMs = std::max(6.0f, r.attack.decayMs * (0.85f - jitter * 0.2f));
        r.body.pitchMs = 14.0f + jitter * 18.0f + spread;
    }
    if (has(LabCharacter::Distorted))
    {
        r.drive = std::min(8.0f, r.drive + 0.9f + spread * 0.25f);
        r.dirtBlend = std::min(1.0f, 0.35f + jitter * 0.55f);
        r.fold = std::min(0.5f, 0.05f + jitter * 0.25f);
        r.samplerGrit12Bit = std::min(0.9f, 0.2f + jitter * 0.5f);
    }
    if (has(LabCharacter::Memphis) || has(LabCharacter::Dark))
    {
        r.destruction = DestructionTier::Destroyed;
        r.reverbMix = std::min(0.25f, 0.04f + jitter * 0.12f);
        r.body.decayMs *= (spec.shortTails ? (0.45f + jitter * 0.25f) : (0.75f + jitter * 0.2f));
        r.texture.enabled = true;
    }
    if (has(LabCharacter::Bright))
        r.stereoWidth = std::min(0.55f, r.stereoWidth + 0.12f);
    if (has(LabCharacter::Minimal))
    {
        r.texture.enabled = false;
        r.reverbMix = 0.02f;
        r.dirtBlend *= 0.5f;
    }
    if (spec.gritMid)
    {
        r.dirtBlend = std::min(1.0f, r.dirtBlend + 0.15f + jitter * 0.2f);
        r.fold = std::min(0.45f, r.fold + 0.04f + jitter * 0.08f);
    }
    if (spec.shortTails || spec.mpcOptimized)
        r.body.decayMs = std::clamp(r.body.decayMs * (0.5f + jitter * 0.35f), 35.0f, 260.0f);
    if (spec.mpcOptimized)
    {
        r.stereoWidth = std::min(r.stereoWidth, 0.08f + jitter * 0.1f);
        r.reverbMix = std::min(r.reverbMix, 0.06f);
        r.outputGain = 0.82f + jitter * 0.12f;
    }

    r.masterSeed = spec.seed ^ (variant * 0x9E3779B97F4A7C15ULL);
    r.body.seedOffset = static_cast<uint32_t>(11u + variant * 17u);
    r.attack.seedOffset = static_cast<uint32_t>(29u + variant * 23u);
    r.texture.seedOffset = static_cast<uint32_t>(53u + variant * 31u);
    r.body.pitchStart = r.rootHz * (1.8f + jitter * 1.6f + spread * 0.05f);
}

inline float scoreShot(const PromptSpec& spec, const DrumRecipe& r, const export_wav::QaReport& qa) noexcept
{
    if (qa.hasNan || qa.silent || qa.peak < 0.01f)
        return -1.0e6f;
    float score = qa.peak * 10.0f;
    if (qa.clipped)
        score -= 1.5f; // hard Memphis kicks often clip — penalize lightly
    if (std::fabs(qa.dc) > 0.05f)
        score -= 2.0f;
    if (spec.mpcOptimized && r.body.decayMs < 180.0f)
        score += 1.5f;
    if (spec.gritMid && r.dirtBlend > 0.5f)
        score += 1.0f;
    for (int i = 0; i < spec.characterCount; ++i)
    {
        if (spec.characters[i] == LabCharacter::Distorted && r.drive > 3.5f)
            score += 1.2f;
        if (spec.characters[i] == LabCharacter::Hard && r.drive > 3.0f)
            score += 0.8f;
    }
    return score;
}

inline void makeFilename(char* out, size_t outN, const PromptSpec& spec, int index, float score) noexcept
{
    const char* kind = "Kick";
    if (spec.instrument == LabInstrument::Bass808)
        kind = "808";
    else if (spec.instrument == LabInstrument::Snare)
        kind = "Snare";
    const int scorePct = std::clamp(static_cast<int>(score * 10.0f), 0, 99);
    std::snprintf(out, outN, "DLZ_%s_%s_%02d_s%02d.wav", kind, spec.rootName, index + 1, scorePct);
}

/**
 * Generate N one-shots from a prompt: mutate recipes, render offline, QA, rank, keep top.
 * Oversamples candidates (count * 2) then keeps best `count` that pass QA.
 */
inline std::vector<RankedShot> generateFromPrompt(const PromptSpec& spec, double sampleRate = 48000.0) noexcept
{
    const int want = std::clamp(spec.count, 1, 64);
    const int candidates = std::min(128, want * 2);
    std::vector<RankedShot> pool;
    pool.reserve(static_cast<size_t>(candidates));

    for (int i = 0; i < candidates; ++i)
    {
        RankedShot shot;
        shot.recipe = baseRecipeFor(spec);
        applyCharacters(shot.recipe, spec, static_cast<uint64_t>(i + 1));

        auto buf = ThreeLayerVoice::renderOffline(shot.recipe, sampleRate, 0.95f);
        shot.frames = static_cast<int>(buf.size());
        shot.interleaved.resize(static_cast<size_t>(shot.frames) * 2u);
        for (int f = 0; f < shot.frames; ++f)
        {
            shot.interleaved[static_cast<size_t>(f) * 2u] = buf[static_cast<size_t>(f)].left;
            shot.interleaved[static_cast<size_t>(f) * 2u + 1u] = buf[static_cast<size_t>(f)].right;
        }
        shot.qa = export_wav::analyzeInterleavedStereo(shot.interleaved.data(), shot.frames);
        shot.score = scoreShot(spec, shot.recipe, shot.qa);
        makeFilename(shot.filename, sizeof(shot.filename), spec, i, shot.score);
        pool.push_back(std::move(shot));
    }

    std::sort(pool.begin(), pool.end(),
              [](const RankedShot& a, const RankedShot& b) { return a.score > b.score; });

    std::vector<RankedShot> kept;
    kept.reserve(static_cast<size_t>(want));
    for (auto& s : pool)
    {
        if (s.score < 0.0f)
            continue;
        bool dup = false;
        for (const auto& k : kept)
        {
            // Same master seed only — character spreads intentionally
            if (k.recipe.masterSeed == s.recipe.masterSeed)
            {
                dup = true;
                break;
            }
        }
        if (dup)
            continue;
        makeFilename(s.filename, sizeof(s.filename), spec, static_cast<int>(kept.size()), s.score);
        kept.push_back(std::move(s));
        if (static_cast<int>(kept.size()) >= want)
            break;
    }
    return kept;
}

} // namespace disklordz::lab
