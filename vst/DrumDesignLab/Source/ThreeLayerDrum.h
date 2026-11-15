#pragma once

#include "CinematicDSP.h"
#include "DSP/CoreMathAndPRNG.h"
#include "DSP/PhysicalModels.h"
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace disklordz
{

enum class InstrumentFamily : uint8_t
{
    // Core Trap / Street / Electronic Drum Classes
    Bass808 = 0,
    Kick,
    Snare,
    Clap,
    ClosedHat,
    OpenHat,
    RimTom,
    FmBellPerc,
    SyntheticFoleyFx,
    // Seven Flagship Percussion Forge Pro Engines
    TimpaniForge,
    WarDrumForge,
    ConcertBassForge,
    MilitarySnareForge,
    TomForge,
    CymbalForge,
    CinematicImpactForge,
    Count
};

enum class BodyGenMode : uint8_t
{
    AnalogSineTri = 0,
    PolyBlepPulseSaw,
    FmTwoOp,
    MembraneModal,
    MetallicModal
};

enum class TextureGenMode : uint8_t
{
    FilteredNoise = 0,
    SixOscMetallic,
    MultiBurstClap,
    MetallicPlateBell,
    SnareWireRattle,
    ReverseSuctionSwell
};

enum class DestructionTier : uint8_t
{
    Clean = 0,
    Mixed,
    Destroyed
};

struct LayerRecipe
{
    bool     enabled    = true;
    bool     muted      = false;
    bool     solo       = false;
    bool     locked     = false;
    uint8_t  genMode    = 0;
    float    level      = 0.80f; // 0..1.25
    float    pan        = 0.0f;  // -1..+1
    float    tuneHz     = 49.0f;
    float    pitchStart = 165.0f;
    float    pitchMs    = 24.0f;
    float    decayMs    = 360.0f;
    float    cutoffHz   = 8500.0f;
    float    resonance  = 0.75f;
    float    morphOrPos = 0.25f; // waveform morph or membrane strike position
    float    fmRatio    = 2.50f;
    float    fmIndex    = 1.20f;
    uint32_t seedOffset = 101u;
};

// Portable, versioned, POD-safe immutable sound recipe (safe for lock-free double-buffer handoff).
struct DrumRecipe
{
    uint32_t         schemaVersion    = 1u;
    InstrumentFamily family           = InstrumentFamily::Bass808;
    DestructionTier  destruction      = DestructionTier::Mixed;
    bool             pitched          = true;
    float            rootHz           = 48.9994f; // G1 default
    float            glideMs          = 0.0f;
    float            subCrossoverHz   = 115.0f;
    float            drive            = 2.6f;
    float            fold             = 0.08f;
    float            dirtBlend        = 0.62f;
    float            samplerGrit12Bit = 0.0f; // 0..1 (26.04 kHz / 12-bit ZOH + 11.5 kHz LPF)
    float            shellSize        = 0.55f;
    float            shellCoupling    = 0.25f;
    float            stereoWidth      = 0.28f;
    float            reverbMix        = 0.08f;
    float            roomSize         = 0.42f;
    float            outputGain       = 0.85f;
    uint64_t         masterSeed       = 313808ULL;

    LayerRecipe body {};
    LayerRecipe attack {};
    LayerRecipe texture {};
};

struct PadProgram
{
    int        midiNote   = 36;
    int        chokeGroup = 0; // 0 = off, 1 = hats, 2 = 808 bass
    float      padGain    = 1.0f;
    DrumRecipe recipe {};
};

struct KitProgram
{
    static constexpr size_t kPadCount = 16;
    uint32_t                        version = 1u;
    std::array<char, 48>            bankName { "Cinematic Shadows - Dark" };
    std::array<PadProgram, kPadCount> pads {};
};

// Build factory-calibrated 3-layer recipe for any of the 16 InstrumentFamily types & Cinematic moods.
inline DrumRecipe makeFactoryRecipe(
    InstrumentFamily family,
    cinematic::Mood  mood = cinematic::Mood::Dark,
    uint64_t         seed = 313808ULL) noexcept
{
    const auto mp = cinematic::kMoodProfiles[static_cast<size_t>(mood) % 5u];
    DrumRecipe r {};
    r.family      = family;
    r.masterSeed  = seed;
    r.rootHz      = mp.subHz;
    r.drive       = mp.drive;
    r.reverbMix   = mp.spaceMix;
    r.stereoWidth = 0.25f + 0.25f * mp.brightness;

    // Layer defaults
    r.body.enabled       = true;
    r.body.seedOffset    = 11u;
    r.attack.enabled     = true;
    r.attack.seedOffset  = 29u;
    r.texture.enabled    = true;
    r.texture.seedOffset = 53u;

    switch (family)
    {
        case InstrumentFamily::Bass808:
            r.pitched            = true;
            r.body.genMode       = static_cast<uint8_t>(BodyGenMode::AnalogSineTri);
            r.body.tuneHz        = mp.subHz;
            r.body.pitchStart    = mp.subHz * mp.pitchMult;
            r.body.pitchMs       = 26.0f;
            r.body.decayMs       = 560.0f * mp.decayScale;
            r.body.morphOrPos    = 0.22f;
            r.body.level         = 0.92f;
            r.attack.genMode     = static_cast<uint8_t>(dsp::ProceduralExciter::Type::HardMallet);
            r.attack.tuneHz      = 1650.0f;
            r.attack.morphOrPos  = 0.68f;
            r.attack.level       = 0.28f;
            r.texture.genMode    = static_cast<uint8_t>(TextureGenMode::FilteredNoise);
            r.texture.cutoffHz   = 2600.0f;
            r.texture.decayMs    = 45.0f;
            r.texture.level      = 0.14f;
            r.reverbMix          = 0.0f; // Keep 808 sub tight & dry by default
            break;

        case InstrumentFamily::Kick:
            r.pitched            = false;
            r.rootHz             = 54.0f;
            r.body.genMode       = static_cast<uint8_t>(BodyGenMode::AnalogSineTri);
            r.body.tuneHz        = 54.0f;
            r.body.pitchStart    = 195.0f;
            r.body.pitchMs       = 16.0f;
            r.body.decayMs       = 155.0f * mp.decayScale;
            r.body.level         = 0.95f;
            r.attack.genMode     = static_cast<uint8_t>(dsp::ProceduralExciter::Type::FeltBeater);
            r.attack.morphOrPos  = 0.78f;
            r.attack.tuneHz      = 2400.0f;
            r.attack.level       = 0.46f;
            r.texture.genMode    = static_cast<uint8_t>(TextureGenMode::FilteredNoise);
            r.texture.cutoffHz   = 3200.0f;
            r.texture.decayMs    = 28.0f;
            r.texture.level      = 0.18f;
            r.reverbMix          = 0.02f;
            break;

        case InstrumentFamily::Snare:
        case InstrumentFamily::MilitarySnareForge:
            r.pitched            = false;
            r.rootHz             = 195.0f;
            r.body.genMode       = static_cast<uint8_t>(BodyGenMode::MembraneModal);
            r.body.tuneHz        = (family == InstrumentFamily::MilitarySnareForge) ? 218.0f : 192.0f;
            r.body.decayMs       = 95.0f * mp.decayScale;
            r.body.morphOrPos    = 0.38f;
            r.body.level         = 0.74f;
            r.shellSize          = 0.28f;
            r.shellCoupling      = 0.36f;
            r.attack.genMode     = static_cast<uint8_t>(dsp::ProceduralExciter::Type::Drumstick);
            r.attack.morphOrPos  = 0.82f;
            r.attack.level       = 0.55f;
            r.texture.genMode    = static_cast<uint8_t>(TextureGenMode::SnareWireRattle);
            r.texture.cutoffHz   = 4200.0f + 2500.0f * mp.brightness;
            r.texture.decayMs    = (family == InstrumentFamily::MilitarySnareForge ? 165.0f : 115.0f) * mp.decayScale;
            r.texture.level      = 0.78f;
            break;

        case InstrumentFamily::Clap:
            r.pitched            = false;
            r.body.genMode       = static_cast<uint8_t>(BodyGenMode::MembraneModal);
            r.body.tuneHz        = 310.0f;
            r.body.decayMs       = 38.0f;
            r.body.level         = 0.26f;
            r.attack.genMode     = static_cast<uint8_t>(dsp::ProceduralExciter::Type::MultiHitBurst);
            r.attack.morphOrPos  = 0.72f;
            r.attack.level       = 0.85f;
            r.texture.genMode    = static_cast<uint8_t>(TextureGenMode::MultiBurstClap);
            r.texture.cutoffHz   = 1850.0f + 2400.0f * mp.brightness;
            r.texture.decayMs    = 95.0f * mp.decayScale;
            r.texture.level      = 0.82f;
            break;

        case InstrumentFamily::ClosedHat:
        case InstrumentFamily::OpenHat:
        case InstrumentFamily::CymbalForge:
        {
            const bool isLong = (family != InstrumentFamily::ClosedHat);
            r.pitched         = false;
            r.body.genMode    = static_cast<uint8_t>(BodyGenMode::MetallicModal);
            r.body.tuneHz     = (family == InstrumentFamily::CymbalForge) ? 340.0f : 520.0f;
            r.body.decayMs    = (family == InstrumentFamily::CymbalForge ? 980.0f : (isLong ? 290.0f : 34.0f)) * mp.decayScale;
            r.body.level      = 0.62f;
            r.attack.genMode  = static_cast<uint8_t>(dsp::ProceduralExciter::Type::MetalStick);
            r.attack.morphOrPos = 0.85f;
            r.attack.level    = 0.44f;
            r.texture.genMode = static_cast<uint8_t>(TextureGenMode::SixOscMetallic);
            r.texture.cutoffHz = 7200.0f;
            r.texture.decayMs = r.body.decayMs;
            r.texture.level   = 0.76f;
            break;
        }

        case InstrumentFamily::TimpaniForge:
            r.pitched            = true;
            r.rootHz             = 73.4162f; // D2 Concert Timpani
            r.body.genMode       = static_cast<uint8_t>(BodyGenMode::MembraneModal);
            r.body.tuneHz        = r.rootHz;
            r.body.pitchStart    = r.rootHz * 1.12f;
            r.body.decayMs       = 1150.0f * mp.decayScale;
            r.body.morphOrPos    = 0.72f; // Rim-weighted timpani strike
            r.body.level         = 0.94f;
            r.shellSize          = 0.82f;
            r.shellCoupling      = 0.45f;
            r.attack.genMode     = static_cast<uint8_t>(dsp::ProceduralExciter::Type::FeltBeater);
            r.attack.morphOrPos  = 0.48f;
            r.attack.level       = 0.42f;
            r.texture.genMode    = static_cast<uint8_t>(TextureGenMode::FilteredNoise);
            r.texture.cutoffHz   = 950.0f;
            r.texture.decayMs    = 140.0f;
            r.texture.level      = 0.16f;
            r.drive              = 1.4f;
            r.reverbMix          = 0.18f;
            break;

        case InstrumentFamily::WarDrumForge:
        case InstrumentFamily::ConcertBassForge:
            r.pitched            = false;
            r.rootHz             = (family == InstrumentFamily::ConcertBassForge) ? 44.0f : 58.0f;
            r.body.genMode       = static_cast<uint8_t>(BodyGenMode::MembraneModal);
            r.body.tuneHz        = r.rootHz;
            r.body.pitchStart    = r.rootHz * 1.65f;
            r.body.decayMs       = (family == InstrumentFamily::ConcertBassForge ? 1280.0f : 780.0f) * mp.decayScale;
            r.body.morphOrPos    = 0.25f;
            r.body.level         = 0.96f;
            r.shellSize          = 0.92f;
            r.shellCoupling      = 0.58f;
            r.attack.genMode     = static_cast<uint8_t>(dsp::ProceduralExciter::Type::SoftMallet);
            r.attack.morphOrPos  = (family == InstrumentFamily::WarDrumForge) ? 0.74f : 0.38f;
            r.attack.level       = 0.56f;
            r.texture.genMode    = static_cast<uint8_t>(TextureGenMode::FilteredNoise);
            r.texture.cutoffHz   = 1800.0f;
            r.texture.decayMs    = 240.0f;
            r.texture.level      = 0.28f;
            r.reverbMix          = 0.22f;
            break;

        case InstrumentFamily::RimTom:
        case InstrumentFamily::TomForge:
            r.pitched            = true;
            r.rootHz             = (family == InstrumentFamily::TomForge) ? 110.0f : 465.0f;
            r.body.genMode       = static_cast<uint8_t>(BodyGenMode::MembraneModal);
            r.body.tuneHz        = r.rootHz;
            r.body.decayMs       = (family == InstrumentFamily::TomForge) ? 420.0f : 95.0f;
            r.body.morphOrPos    = 0.45f;
            r.body.level         = 0.86f;
            r.shellSize          = 0.50f;
            r.shellCoupling      = 0.38f;
            r.attack.genMode     = static_cast<uint8_t>(dsp::ProceduralExciter::Type::WoodenStick);
            r.attack.morphOrPos  = 0.80f;
            r.attack.level       = 0.52f;
            r.texture.genMode    = static_cast<uint8_t>(TextureGenMode::FilteredNoise);
            r.texture.cutoffHz   = 2800.0f;
            r.texture.decayMs    = 55.0f;
            r.texture.level      = 0.20f;
            break;

        case InstrumentFamily::FmBellPerc:
        case InstrumentFamily::SyntheticFoleyFx:
        case InstrumentFamily::CinematicImpactForge:
        case InstrumentFamily::Count:
            r.pitched            = (family == InstrumentFamily::FmBellPerc);
            r.rootHz             = (family == InstrumentFamily::CinematicImpactForge) ? 42.0f : 523.25f;
            r.body.genMode       = static_cast<uint8_t>(BodyGenMode::FmTwoOp);
            r.body.tuneHz        = r.rootHz;
            r.body.pitchStart    = r.rootHz * 2.2f;
            r.body.pitchMs       = 38.0f;
            r.body.decayMs       = (family == InstrumentFamily::CinematicImpactForge ? 1350.0f : 380.0f) * mp.decayScale;
            r.body.fmRatio       = 2.718f;
            r.body.fmIndex       = mp.fmAmount * 1.4f;
            r.body.level         = 0.84f;
            r.attack.genMode     = static_cast<uint8_t>(dsp::ProceduralExciter::Type::MetalStick);
            r.attack.morphOrPos  = 0.88f;
            r.attack.level       = 0.58f;
            r.texture.genMode    = static_cast<uint8_t>(TextureGenMode::MetallicPlateBell);
            r.texture.cutoffHz   = 5400.0f;
            r.texture.decayMs    = 620.0f * mp.decayScale;
            r.texture.level      = 0.52f;
            r.reverbMix          = 0.24f;
            break;
    }
    return r;
}

// Independent runtime synthesis state for a single active hit (zero heap allocation, zero shared state).
class ThreeLayerVoice
{
public:
    void prepare(double sampleRate) noexcept
    {
        sr_ = std::clamp(sampleRate, 8000.0, 192000.0);
        dirtShaperL_.prepare(sr_);
        dirtShaperR_.prepare(sr_);
        dcL_.prepare(sr_, 20.0);
        dcR_.prepare(sr_, 20.0);
    }

    void trigger(
        const DrumRecipe& recipe,
        double            sampleRate,
        float             velocity01,
        int               midiNote       = -1,
        int               chokeGroup     = 0,
        uint64_t          triggerCounter = 1u,
        float             lastRootHz     = 0.0f) noexcept
    {
        prepare(sampleRate);
        recipe_         = recipe;
        velocity_       = std::clamp(velocity01, 0.05f, 1.0f);
        chokeGroup_     = chokeGroup;
        age_            = triggerCounter;
        chokeFadeGain_  = 1.0f;
        choking_        = false;
        chokeStep_      = 1.0f / std::max(16.0f, static_cast<float>(0.0025 * sr_)); // 2.5 ms linear de-click fade
        sampleIdx_      = 0;

        // Independent deterministic PRNG streams per layer: muting/locking Attack never alters Body or Texture!
        bodyRng_.reseed(recipe_.masterSeed, 0xB0D1ULL + recipe_.body.seedOffset);
        attackRng_.reseed(recipe_.masterSeed, 0xA77AULL + recipe_.attack.seedOffset);
        textureRng_.reseed(recipe_.masterSeed, 0x7E87ULL + recipe_.texture.seedOffset);

        targetRootHz_ = recipe_.body.tuneHz;
        if (recipe_.pitched && midiNote >= 12 && midiNote <= 108)
            targetRootHz_ = static_cast<float>(440.0 * std::pow(2.0, (midiNote - 69) / 12.0));
        targetRootHz_ = std::clamp(targetRootHz_, 22.0f, static_cast<float>(sr_ * 0.42));

        currentBaseHz_ = (recipe_.glideMs > 1.0f && lastRootHz > 20.0f) ? lastRootHz : targetRootHz_;
        glideCoeff_    = (recipe_.glideMs > 1.0f)
                           ? static_cast<float>(1.0 - std::exp(-1.0 / (recipe_.glideMs * 0.001 * sr_)))
                           : 1.0f;

        const double maxLayerDecaySec = std::max({ recipe_.body.decayMs, recipe_.attack.decayMs, recipe_.texture.decayMs }) * 0.001;
        const double totalSec         = std::clamp(maxLayerDecaySec * 3.8 + 0.08, 0.12, 6.5);
        totalSamples_                 = std::max<int64_t>(128, static_cast<int64_t>(std::ceil(sr_ * totalSec)));

        // Reset Body state
        bodyPhase_    = 0.0;
        bodyModPhase_ = 0.0;
        bodyAmpEnv_   = 1.0f;
        bodyAmpDecay_ = static_cast<float>(std::exp(-1.0 / (std::max(8.0f, recipe_.body.decayMs) * 0.001 * sr_)));
        pitchEnv_     = std::max(0.0f, recipe_.body.pitchStart - targetRootHz_);
        pitchDecay_   = static_cast<float>(std::exp(-1.0 / (std::max(2.0f, recipe_.body.pitchMs) * 0.001 * sr_)));

        membrane_.trigger(sr_, targetRootHz_, recipe_.body.decayMs * 0.001f, recipe_.body.morphOrPos, 3.2f, velocity_);
        shell_.configure(sr_, recipe_.shellSize, 0.55f, recipe_.shellCoupling);
        bodyMetal_.trigger(sr_, targetRootHz_, recipe_.body.decayMs * 0.001f, dsp::MetallicResonator::Material::Cymbal, 0.72f, velocity_);
        bodyFilter_.setParams(sr_, std::clamp(recipe_.body.cutoffHz, 60.0f, static_cast<float>(sr_ * 0.44)), std::max(0.5f, recipe_.body.resonance), dsp::TptSvf::Mode::LowPass);
        bodyFilter_.reset();

        // Reset Attack exciter
        const auto excType = static_cast<dsp::ProceduralExciter::Type>(recipe_.attack.genMode % 8u);
        exciter_.trigger(sr_, excType, recipe_.attack.morphOrPos, velocity_, recipe_.attack.tuneHz);

        // Reset Texture state
        texEnv_   = 1.0f;
        texDecay_ = static_cast<float>(std::exp(-1.0 / (std::max(8.0f, recipe_.texture.decayMs) * 0.001 * sr_)));
        texMetal_.trigger(sr_, std::max(220.0f, recipe_.texture.cutoffHz * 0.25f), recipe_.texture.decayMs * 0.001f, dsp::MetallicResonator::Material::TubularBell, 0.8f, velocity_);
        const auto texMode = (static_cast<TextureGenMode>(recipe_.texture.genMode % 6u) == TextureGenMode::SixOscMetallic)
                               ? dsp::TptSvf::Mode::HighPass
                               : dsp::TptSvf::Mode::BandPass;
        texFilterL_.setParams(sr_, recipe_.texture.cutoffHz, 1.15, texMode);
        texFilterR_.setParams(sr_, recipe_.texture.cutoffHz * 1.03, 1.15, texMode);
        texFilterL_.reset();
        texFilterR_.reset();
        sixOscPhases_ = { 0.0, 0.17, 0.31, 0.49, 0.67, 0.83 };

        // Split-band crossover & vintage 12-bit state
        const double xover = std::clamp(static_cast<double>(recipe_.subCrossoverHz), 55.0, 260.0);
        subLp_.setParams(sr_, xover, 0.707, dsp::TptSvf::Mode::LowPass);
        dirtHpL_.setParams(sr_, xover, 0.707, dsp::TptSvf::Mode::HighPass);
        dirtHpR_.setParams(sr_, xover, 0.707, dsp::TptSvf::Mode::HighPass);
        reconLpL_.setParams(sr_, std::min(11500.0, sr_ * 0.44), 0.707, dsp::TptSvf::Mode::LowPass);
        reconLpR_.setParams(sr_, std::min(11500.0, sr_ * 0.44), 0.707, dsp::TptSvf::Mode::LowPass);
        subLp_.reset(); dirtHpL_.reset(); dirtHpR_.reset(); reconLpL_.reset(); reconLpR_.reset();
        dirtShaperL_.reset(); dirtShaperR_.reset();
        dcL_.reset(); dcR_.reset();
        zohCounter_ = 0.0; heldL_ = heldR_ = 0.0f;

        active_ = true;
    }

    void beginChokeFade() noexcept { choking_ = true; }
    bool isActive() const noexcept { return active_; }
    int chokeGroup() const noexcept { return chokeGroup_; }
    uint64_t age() const noexcept { return age_; }
    float currentEnvelopeApprox() const noexcept { return std::max(bodyAmpEnv_, texEnv_) * chokeFadeGain_; }
    float targetRootHz() const noexcept { return recipe_.pitched ? targetRootHz_ : 0.0f; }
    int64_t totalSamples() const noexcept { return totalSamples_; }

    dsp::StereoSample renderSample() noexcept
    {
        if (!active_ || sampleIdx_ >= totalSamples_)
        {
            active_ = false;
            return {};
        }

        const double t = static_cast<double>(sampleIdx_) / sr_;
        currentBaseHz_ += glideCoeff_ * (targetRootHz_ - currentBaseHz_);

        // Always advance each layer's isolated PRNG deterministically on its own path
        const float atkExc = exciter_.process(attackRng_);

        // 1. BODY LAYER
        float bodySig = 0.0f;
        const auto bMode = static_cast<BodyGenMode>(recipe_.body.genMode % 5u);
        const double instHz = std::clamp(static_cast<double>(currentBaseHz_ + pitchEnv_), 18.0, sr_ * 0.44);
        const double dt     = instHz / sr_;
        bodyPhase_ += dt;
        if (bodyPhase_ >= 1.0) bodyPhase_ -= 1.0;
        const float atkRamp = static_cast<float>(std::min(1.0, t / 0.0008));

        switch (bMode)
        {
            case BodyGenMode::AnalogSineTri:
            {
                const float s   = static_cast<float>(std::sin(dsp::kTwoPi * bodyPhase_));
                const float tri = static_cast<float>((2.0 / dsp::kPi) * std::asin(std::sin(dsp::kTwoPi * bodyPhase_)));
                const float m   = std::clamp(recipe_.body.morphOrPos, 0.0f, 1.0f);
                bodySig         = ((1.0f - m) * s + m * tri) * bodyAmpEnv_ * atkRamp;
                break;
            }
            case BodyGenMode::PolyBlepPulseSaw:
            {
                const float saw = dsp::blepSaw(bodyPhase_, dt);
                const float pls = dsp::blepPulse(bodyPhase_, dt, 0.35 + 0.3 * recipe_.body.morphOrPos);
                bodySig         = (0.55f * saw + 0.45f * pls) * bodyAmpEnv_ * atkRamp;
                break;
            }
            case BodyGenMode::FmTwoOp:
            {
                const double modDt = std::min(instHz * static_cast<double>(recipe_.body.fmRatio), sr_ * 0.44) / sr_;
                bodyModPhase_ += modDt;
                if (bodyModPhase_ >= 1.0) bodyModPhase_ -= 1.0;
                const double idx = static_cast<double>(recipe_.body.fmIndex * bodyAmpEnv_);
                const double mod = std::sin(dsp::kTwoPi * bodyModPhase_) * idx;
                bodySig          = static_cast<float>(std::sin(dsp::kTwoPi * bodyPhase_ + mod)) * bodyAmpEnv_ * atkRamp;
                break;
            }
            case BodyGenMode::MembraneModal:
                bodySig = shell_.process(membrane_.process(atkExc * 0.45f));
                break;
            case BodyGenMode::MetallicModal:
                bodySig = bodyMetal_.process(atkExc * 0.5f);
                break;
        }
        bodySig = bodyFilter_.process(bodySig);
        (void)bodyRng_.nextU32();

        // 2. ATTACK LAYER
        const float attackSig = atkExc;

        // 3. TEXTURE LAYER
        float texL = 0.0f, texR = 0.0f;
        const float nL = textureRng_.nextSigned();
        const float nR = textureRng_.nextSigned();
        const auto tMode = static_cast<TextureGenMode>(recipe_.texture.genMode % 6u);

        switch (tMode)
        {
            case TextureGenMode::FilteredNoise:
                texL = texFilterL_.process(nL) * texEnv_;
                texR = texFilterR_.process(nR) * texEnv_;
                break;
            case TextureGenMode::SixOscMetallic:
            {
                static constexpr std::array<double, 6> kHatHz { 205.3, 304.4, 369.6, 522.7, 540.5, 800.0 };
                float metal = 0.0f;
                for (size_t i = 0; i < 6; ++i)
                {
                    const double f   = std::min(kHatHz[i] * (0.95 + 0.35 * velocity_), sr_ * 0.43);
                    const double hdt = f / sr_;
                    sixOscPhases_[i] += hdt;
                    if (sixOscPhases_[i] >= 1.0) sixOscPhases_[i] -= 1.0;
                    metal += dsp::blepPulse(sixOscPhases_[i], hdt, 0.5) * (1.0f / 6.0f);
                }
                texL = texFilterL_.process(0.58f * metal + 0.42f * nL) * texEnv_;
                texR = texFilterR_.process(0.58f * metal + 0.42f * nR) * texEnv_;
                break;
            }
            case TextureGenMode::MultiBurstClap:
            {
                float burst = 0.0f;
                for (int b = 0; b < 4; ++b)
                {
                    const double onset = b * 0.0095;
                    if (t >= onset)
                        burst += static_cast<float>(std::exp(-(t - onset) / (b == 3 ? (recipe_.texture.decayMs * 0.001) : 0.0055)));
                }
                texL = texFilterL_.process(nL) * burst * 0.65f;
                texR = texFilterR_.process(nR) * burst * 0.65f;
                break;
            }
            case TextureGenMode::MetallicPlateBell:
            {
                const float m = texMetal_.process(atkExc * 0.25f);
                texL          = m + 0.12f * texFilterL_.process(nL) * texEnv_;
                texR          = m * 0.96f + 0.12f * texFilterR_.process(nR) * texEnv_;
                break;
            }
            case TextureGenMode::SnareWireRattle:
            {
                const float buzz = 0.72f + 0.28f * static_cast<float>(std::sin(dsp::kTwoPi * 145.0 * t));
                texL             = texFilterL_.process(nL) * texEnv_ * buzz;
                texR             = texFilterR_.process(nR) * texEnv_ * buzz;
                break;
            }
            case TextureGenMode::ReverseSuctionSwell:
            {
                const double swellDur = std::max(0.08, recipe_.texture.decayMs * 0.001);
                float swell = 0.0f;
                if (t < swellDur)
                    swell = static_cast<float>(std::pow(t / swellDur, 2.4));
                else if (t < swellDur + 0.0025)
                    swell = static_cast<float>(1.0 - (t - swellDur) / 0.0025);
                texL = texFilterL_.process(nL) * swell;
                texR = texFilterR_.process(nR) * swell;
                break;
            }
        }

        // 4. LAYER MIXER (Solo / Mute / Enable / Pan)
        const bool anySolo = recipe_.body.solo || recipe_.attack.solo || recipe_.texture.solo;
        const auto layerGain = [anySolo](const LayerRecipe& lr) noexcept -> float {
            if (!lr.enabled || lr.muted) return 0.0f;
            if (anySolo && !lr.solo) return 0.0f;
            return lr.level;
        };

        const float gB = layerGain(recipe_.body);
        const float gA = layerGain(recipe_.attack);
        const float gT = layerGain(recipe_.texture);

        auto panStereo = [](float monoL, float monoR, float pan, float width) noexcept -> dsp::StereoSample {
            const float p     = std::clamp(pan, -1.0f, 1.0f);
            const float gainL = std::sqrt(0.5f * (1.0f - p));
            const float gainR = std::sqrt(0.5f * (1.0f + p));
            const float mid   = 0.5f * (monoL + monoR);
            const float side  = 0.5f * (monoL - monoR) * (1.0f + width);
            return { (mid + side) * gainL, (mid - side) * gainR };
        };

        const auto bSt = panStereo(bodySig * gB, bodySig * gB, recipe_.body.pan, 0.0f);
        const auto aSt = panStereo(attackSig * gA, attackSig * gA, recipe_.attack.pan, 0.1f);
        const auto tSt = panStereo(texL * gT, texR * gT, recipe_.texture.pan, recipe_.stereoWidth);

        float sumL = bSt.left + aSt.left + tSt.left;
        float sumR = bSt.right + aSt.right + tSt.right;

        // 5. PHASE-COHERENT CLEAN-SUB (<subCrossoverHz MONO) + 4x OVERSAMPLED NONLINEAR DIRT BRANCH
        const float monoIn   = 0.5f * (sumL + sumR);
        const float cleanSub = subLp_.process(monoIn);
        const float highL    = dirtHpL_.process(sumL);
        const float highR    = dirtHpR_.process(sumR);

        float driveScale = 1.0f;
        float dirtMix    = recipe_.dirtBlend;
        float foldAmt    = recipe_.fold;
        if (recipe_.destruction == DestructionTier::Clean)
        {
            driveScale = 0.45f;
            dirtMix *= 0.35f;
            foldAmt = 0.0f;
        }
        else if (recipe_.destruction == DestructionTier::Destroyed)
        {
            driveScale = 1.65f;
            dirtMix    = std::min(1.0f, dirtMix + 0.28f);
            foldAmt    = std::min(0.85f, foldAmt + 0.22f);
        }

        const auto curve = (foldAmt > 0.05f) ? dsp::OversampledWaveshaper4x::Curve::Wavefold
                                             : dsp::OversampledWaveshaper4x::Curve::AsymmetricClip;
        const float dirtL = dirtShaperL_.process(highL + 0.35f * cleanSub, recipe_.drive * driveScale, curve, foldAmt);
        const float dirtR = dirtShaperR_.process(highR + 0.35f * cleanSub, recipe_.drive * driveScale, curve, foldAmt);

        float outL = cleanSub + (1.0f - dirtMix) * highL + dirtMix * dirtL;
        float outR = cleanSub + (1.0f - dirtMix) * highR + dirtMix * dirtR;

        // 6. OPTIONAL 12-BIT / 26.04 kHz VINTAGE HARDWARE SAMPLER GRIT STAGE
        if (recipe_.samplerGrit12Bit > 0.001f)
        {
            const double step = std::max(1.0, sr_ / 26040.0);
            zohCounter_ += 1.0;
            if (zohCounter_ >= step)
            {
                zohCounter_ -= step;
                heldL_ = std::round(outL * 2048.0f) / 2048.0f;
                heldR_ = std::round(outR * 2048.0f) / 2048.0f;
            }
            const float wL  = reconLpL_.process(heldL_);
            const float wR  = reconLpR_.process(heldR_);
            const float mix = std::clamp(recipe_.samplerGrit12Bit, 0.0f, 1.0f);
            outL            = outL + mix * (wL - outL);
            outR            = outR + mix * (wR - outR);
        }

        // 7. DC BLOCKER + CLICKLESS TAIL / CHOKE RAMP
        outL = dcL_.process(outL);
        outR = dcR_.process(outR);

        if (choking_)
        {
            chokeFadeGain_ -= chokeStep_;
            if (chokeFadeGain_ <= 0.0f)
            {
                chokeFadeGain_ = 0.0f;
                active_        = false;
            }
        }

        const int64_t rem      = totalSamples_ - sampleIdx_;
        const int64_t fadeSamp = std::max<int64_t>(48, static_cast<int64_t>(0.006 * sr_));
        const float   tailFade = (rem < fadeSamp) ? (static_cast<float>(rem) / static_cast<float>(fadeSamp)) : 1.0f;

        const float gain = velocity_ * recipe_.outputGain * chokeFadeGain_ * tailFade;
        bodyAmpEnv_ *= bodyAmpDecay_;
        pitchEnv_ *= pitchDecay_;
        texEnv_ *= texDecay_;
        ++sampleIdx_;
        if (sampleIdx_ >= totalSamples_)
            active_ = false;

        return { std::clamp(outL * gain, -3.5f, 3.5f), std::clamp(outR * gain, -3.5f, 3.5f) };
    }

    static std::vector<dsp::StereoSample> renderOffline(
        const DrumRecipe& recipe,
        double            sampleRate = 48000.0,
        float             velocity   = 0.95f,
        int               midiNote   = -1)
    {
        ThreeLayerVoice v;
        v.trigger(recipe, sampleRate, velocity, midiNote);
        dsp::RoomReverb verb;
        verb.prepare(sampleRate);

        std::vector<dsp::StereoSample> buf(static_cast<size_t>(v.totalSamples()));
        for (size_t i = 0; i < buf.size(); ++i)
            buf[i] = verb.process(v.renderSample(), recipe.reverbMix, recipe.roomSize);
        return buf;
    }

private:
    double                       sr_            = 48000.0;
    DrumRecipe                   recipe_ {};
    float                        velocity_      = 0.9f;
    int                          chokeGroup_    = 0;
    uint64_t                     age_           = 0;
    bool                         active_        = false;
    bool                         choking_       = false;
    float                        chokeFadeGain_ = 1.0f;
    float                        chokeStep_     = 0.01f;
    int64_t                      totalSamples_  = 0;
    int64_t                      sampleIdx_     = 0;

    float                        targetRootHz_  = 49.0f;
    float                        currentBaseHz_ = 49.0f;
    float                        glideCoeff_    = 1.0f;
    double                       bodyPhase_     = 0.0;
    double                       bodyModPhase_  = 0.0;
    float                        bodyAmpEnv_    = 0.0f;
    float                        bodyAmpDecay_  = 0.999f;
    float                        pitchEnv_      = 0.0f;
    float                        pitchDecay_    = 0.99f;
    float                        texEnv_        = 0.0f;
    float                        texDecay_      = 0.999f;
    std::array<double, 6>        sixOscPhases_ {};

    double                       zohCounter_    = 0.0;
    float                        heldL_         = 0.0f;
    float                        heldR_         = 0.0f;

    dsp::DeterministicRng        bodyRng_, attackRng_, textureRng_;
    dsp::MembraneModel           membrane_;
    dsp::ShellResonator          shell_;
    dsp::MetallicResonator       bodyMetal_, texMetal_;
    dsp::ProceduralExciter       exciter_;
    dsp::TptSvf                  bodyFilter_, texFilterL_, texFilterR_;
    dsp::TptSvf                  subLp_, dirtHpL_, dirtHpR_, reconLpL_, reconLpR_;
    dsp::OversampledWaveshaper4x dirtShaperL_, dirtShaperR_;
    dsp::OnePoleDcBlocker        dcL_, dcR_;
};

// Real-time-safe 32-voice polyphonic percussion allocator with choke groups & de-click voice stealing.
class DrumVoiceManager
{
public:
    static constexpr size_t kMaxVoices = 32;

    void prepare(double sampleRate) noexcept
    {
        sr_ = std::clamp(sampleRate, 8000.0, 192000.0);
        for (auto& v : voices_)
            v.prepare(sr_);
        reverb_.prepare(sr_);
        reset();
    }

    void reset() noexcept
    {
        for (auto& v : voices_)
            v = ThreeLayerVoice {};
        reverb_.reset();
        counter_    = 0;
        lastBassHz_ = 0.0f;
    }

    void triggerPad(const KitProgram& kit, int padIndex, float velocity01, int midiNoteOverride = -1) noexcept
    {
        if (padIndex < 0 || padIndex >= static_cast<int>(KitProgram::kPadCount))
            return;
        const auto& pad = kit.pads[static_cast<size_t>(padIndex)];

        // Choke active voices sharing a non-zero choke group (2.5 ms linear fade, never hard zero)
        if (pad.chokeGroup > 0)
        {
            for (auto& v : voices_)
            {
                if (v.isActive() && v.chokeGroup() == pad.chokeGroup)
                    v.beginChokeFade();
            }
        }

        // Allocate free slot or steal quietest/oldest active voice
        ThreeLayerVoice* target = nullptr;
        for (auto& v : voices_)
        {
            if (!v.isActive())
            {
                target = &v;
                break;
            }
        }
        if (target == nullptr)
        {
            target = &voices_[0];
            for (auto& v : voices_)
            {
                if (v.currentEnvelopeApprox() < target->currentEnvelopeApprox()
                    || (v.currentEnvelopeApprox() == target->currentEnvelopeApprox() && v.age() < target->age()))
                {
                    target = &v;
                }
            }
        }

        DrumRecipe r = pad.recipe;
        r.outputGain *= pad.padGain;
        const int note = (midiNoteOverride >= 0) ? midiNoteOverride : (r.pitched ? -1 : pad.midiNote);
        target->trigger(r, sr_, velocity01, note, pad.chokeGroup, ++counter_, lastBassHz_);
        if (r.family == InstrumentFamily::Bass808 || r.pitched)
            lastBassHz_ = target->targetRootHz();
    }

    void allNotesOff() noexcept
    {
        for (auto& v : voices_)
            if (v.isActive())
                v.beginChokeFade();
    }

    int activeVoiceCount() const noexcept
    {
        int n = 0;
        for (const auto& v : voices_)
            if (v.isActive())
                ++n;
        return n;
    }

    void renderSubBlock(float* outL, float* outR, int startSample, int numSamples, float globalReverbMix = 0.08f) noexcept
    {
        for (int i = 0; i < numSamples; ++i)
        {
            dsp::StereoSample dry {};
            for (auto& v : voices_)
            {
                if (v.isActive())
                    dry += v.renderSample();
            }
            const auto wet            = reverb_.process(dry, globalReverbMix, 0.45f);
            const int  idx            = startSample + i;
            outL[idx]                 = std::isfinite(wet.left) ? std::clamp(wet.left, -4.0f, 4.0f) : 0.0f;
            if (outR != nullptr)
                outR[idx] = std::isfinite(wet.right) ? std::clamp(wet.right, -4.0f, 4.0f) : 0.0f;
        }
    }

private:
    double                                  sr_         = 48000.0;
    uint64_t                                counter_    = 0;
    float                                   lastBassHz_ = 0.0f;
    std::array<ThreeLayerVoice, kMaxVoices> voices_ {};
    dsp::RoomReverb                         reverb_;
};

inline KitProgram makeDefault16PadKit(cinematic::Mood mood = cinematic::Mood::Dark, uint64_t seed = 313808ULL) noexcept
{
    KitProgram kit {};
    static constexpr std::array<InstrumentFamily, 16> kBankALayout {
        InstrumentFamily::Kick,                // 36 C1
        InstrumentFamily::RimTom,              // 37 C#1
        InstrumentFamily::Snare,               // 38 D1
        InstrumentFamily::Clap,                // 39 D#1
        InstrumentFamily::Bass808,             // 40 E1
        InstrumentFamily::TomForge,            // 41 F1
        InstrumentFamily::ClosedHat,           // 42 F#1 (choke 1)
        InstrumentFamily::Bass808,             // 43 G1
        InstrumentFamily::ClosedHat,           // 44 G#1 (choke 1)
        InstrumentFamily::TimpaniForge,        // 45 A1
        InstrumentFamily::OpenHat,             // 46 A#1 (choke 1)
        InstrumentFamily::WarDrumForge,        // 47 B1
        InstrumentFamily::ConcertBassForge,    // 48 C2
        InstrumentFamily::CymbalForge,         // 49 C#2
        InstrumentFamily::MilitarySnareForge,  // 50 D2
        InstrumentFamily::CinematicImpactForge // 51 D#2
    };

    for (size_t i = 0; i < KitProgram::kPadCount; ++i)
    {
        kit.pads[i].midiNote = 36 + static_cast<int>(i);
        kit.pads[i].recipe   = makeFactoryRecipe(kBankALayout[i], mood, seed + i * 131ULL);
        if (kBankALayout[i] == InstrumentFamily::ClosedHat || kBankALayout[i] == InstrumentFamily::OpenHat)
            kit.pads[i].chokeGroup = 1;
        else if (kBankALayout[i] == InstrumentFamily::Bass808)
            kit.pads[i].chokeGroup = 2;
    }
    return kit;
}

} // namespace disklordz
