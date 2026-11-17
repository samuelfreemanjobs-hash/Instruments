#pragma once
// DiskLordz BANG Drum Lab — 8 DNA banks + hard kick / melodic 808 / raw snare recipes.
#include "../ThreeLayerDrum.h"
#include <array>
#include <cstring>

namespace disklordz::bang
{

enum class Bank : uint8_t
{
    MilliPressure = 0,
    DirtyDigital,
    AtlantaEccentric,
    BossDrums,
    StreetExperimental,
    SouthernFuturism,
    RawMachine,
    MinimalMonster,
    Count
};

inline constexpr std::array<const char*, 8> kBankNames {{
    "Milli Pressure",
    "Dirty Digital",
    "Atlanta Eccentric",
    "Boss Drums",
    "Street Experimental",
    "Southern Futurism",
    "Raw Machine",
    "Minimal Monster"
}};

struct BankProfile
{
    float kickFundHz   = 55.0f;
    float kickStartHz  = 210.0f;
    float kickSweepMs  = 12.0f;
    float kickDecayMs  = 120.0f;
    float kickClick    = 0.75f;
    float kickDrive    = 2.8f;
    float bassRootHz   = 49.0f;
    float bassStartMul = 3.2f;
    float bassDecayMs  = 480.0f;
    float bassMorph    = 0.30f;
    float bassDrive    = 2.4f;
    float snareBodyHz  = 205.0f;
    float snareDecayMs = 75.0f;
    float snareCrack   = 0.85f;
    float snareNoise   = 0.55f;
    float grit         = 0.55f; // vintage digital
    float brightness   = 0.70f;
    float reverb       = 0.02f;
};

inline constexpr std::array<BankProfile, 8> kProfiles {{
    // Milli Pressure — relentless, dry, hypnotic
    { 52.f, 220.f, 10.f, 110.f, 0.80f, 3.0f, 46.25f, 3.4f, 420.f, 0.28f, 2.6f, 210.f, 70.f, 0.88f, 0.48f, 0.45f, 0.72f, 0.01f },
    // Dirty Digital — crunchy MPC-era
    { 54.f, 230.f, 8.f,  100.f, 0.90f, 3.4f, 49.f,   3.0f, 380.f, 0.35f, 2.9f, 220.f, 65.f, 0.92f, 0.60f, 0.78f, 0.80f, 0.02f },
    // Atlanta Eccentric — unusual / rubbery
    { 48.f, 180.f, 18.f, 140.f, 0.55f, 2.2f, 55.f,   2.8f, 560.f, 0.42f, 2.1f, 190.f, 95.f, 0.70f, 0.70f, 0.40f, 0.65f, 0.04f },
    // Boss Drums — bold, spacious, minimal
    { 50.f, 200.f, 14.f, 150.f, 0.70f, 2.6f, 43.65f, 3.1f, 500.f, 0.22f, 2.3f, 198.f, 80.f, 0.82f, 0.45f, 0.35f, 0.68f, 0.03f },
    // Street Experimental — distorted / hollow
    { 46.f, 250.f, 9.f,  90.f,  0.85f, 3.8f, 41.2f,  3.6f, 340.f, 0.50f, 3.2f, 230.f, 55.f, 0.95f, 0.75f, 0.70f, 0.85f, 0.02f },
    // Southern Futurism — FM / melodic 808
    { 55.f, 195.f, 16.f, 130.f, 0.60f, 2.4f, 51.91f, 2.6f, 640.f, 0.25f, 2.0f, 185.f, 88.f, 0.75f, 0.50f, 0.30f, 0.75f, 0.05f },
    // Raw Machine — short hard / mechanical
    { 58.f, 240.f, 6.f,  85.f,  0.95f, 3.2f, 49.f,   3.5f, 300.f, 0.20f, 2.8f, 215.f, 55.f, 0.90f, 0.40f, 0.65f, 0.82f, 0.01f },
    // Minimal Monster — sparse, huge transient, deep bass
    { 42.f, 160.f, 22.f, 200.f, 0.65f, 2.5f, 36.71f, 2.9f, 720.f, 0.15f, 2.2f, 175.f, 100.f, 0.78f, 0.35f, 0.25f, 0.55f, 0.04f }
}};

inline BankProfile profileFor(Bank bank) noexcept
{
    return kProfiles[static_cast<size_t>(bank) % 8u];
}

inline void setBankName(KitProgram& kit, Bank bank) noexcept
{
    const char* name = kBankNames[static_cast<size_t>(bank) % 8u];
    std::memset(kit.bankName.data(), 0, kit.bankName.size());
    for (size_t i = 0; i + 1 < kit.bankName.size() && name[i] != '\0'; ++i)
        kit.bankName[i] = name[i];
}

/** Bang Kick Forge — hard, short, aggressive. */
inline DrumRecipe makeKick(Bank bank, uint64_t seed = 0xBA110001ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r {};
    r.schemaVersion      = 1u;
    r.family             = InstrumentFamily::Kick;
    r.destruction        = (p.grit > 0.5f) ? DestructionTier::Mixed : DestructionTier::Clean;
    r.pitched            = false;
    r.rootHz             = p.kickFundHz;
    r.masterSeed         = seed;
    r.drive              = p.kickDrive;
    r.fold               = 0.06f + 0.08f * p.grit;
    r.dirtBlend          = 0.35f + 0.40f * p.kickClick;
    r.samplerGrit12Bit   = p.grit;
    r.reverbMix          = p.reverb;
    r.outputGain         = 0.90f;

    r.body.enabled       = true;
    r.body.genMode       = static_cast<uint8_t>(BodyGenMode::AnalogSineTri);
    r.body.tuneHz        = p.kickFundHz;
    r.body.pitchStart    = p.kickStartHz;
    r.body.pitchMs       = p.kickSweepMs;
    r.body.decayMs       = p.kickDecayMs;
    r.body.morphOrPos    = 0.28f + 0.20f * p.kickClick;
    r.body.level         = 0.96f;
    r.body.cutoffHz      = 5000.0f;
    r.body.seedOffset    = 11u;

    r.attack.enabled     = true;
    r.attack.genMode     = static_cast<uint8_t>(dsp::ProceduralExciter::Type::HardMallet);
    r.attack.tuneHz      = 2200.0f + 1400.0f * p.brightness;
    r.attack.decayMs     = 4.0f + 8.0f * p.kickClick;
    r.attack.morphOrPos  = 0.75f + 0.20f * p.kickClick;
    r.attack.level       = 0.45f + 0.40f * p.kickClick;
    r.attack.seedOffset  = 29u;

    r.texture.enabled    = true;
    r.texture.genMode    = static_cast<uint8_t>(TextureGenMode::FilteredNoise);
    r.texture.cutoffHz   = 2800.0f + 1800.0f * p.brightness;
    r.texture.decayMs    = 16.0f;
    r.texture.level      = 0.10f + 0.18f * p.grit;
    r.texture.seedOffset = 53u;
    return r;
}

/** Melodic 808 Reactor — tuned movement, digital color optional. */
inline DrumRecipe makeBass(Bank bank, float rootHz = 0.0f, uint64_t seed = 0xBA110808ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r {};
    r.schemaVersion      = 1u;
    r.family             = InstrumentFamily::Bass808;
    r.destruction        = (p.grit > 0.6f) ? DestructionTier::Mixed : DestructionTier::Clean;
    r.pitched            = true;
    r.rootHz             = (rootHz > 20.0f) ? rootHz : p.bassRootHz;
    r.masterSeed         = seed;
    r.glideMs            = (bank == Bank::SouthernFuturism || bank == Bank::MilliPressure) ? 55.0f : 12.0f;
    r.drive              = p.bassDrive;
    r.fold               = 0.05f + 0.10f * p.grit;
    r.dirtBlend          = 0.22f + 0.30f * p.grit;
    r.samplerGrit12Bit   = p.grit * 0.85f;
    r.subCrossoverHz     = 105.0f;
    r.reverbMix          = 0.0f;
    r.outputGain         = 0.92f;

    r.body.enabled       = true;
    r.body.genMode       = (bank == Bank::SouthernFuturism || bank == Bank::AtlantaEccentric)
                               ? static_cast<uint8_t>(BodyGenMode::FmTwoOp)
                               : static_cast<uint8_t>(BodyGenMode::AnalogSineTri);
    r.body.tuneHz        = r.rootHz;
    r.body.pitchStart    = r.rootHz * p.bassStartMul;
    r.body.pitchMs       = 22.0f;
    r.body.decayMs       = p.bassDecayMs;
    r.body.morphOrPos    = p.bassMorph;
    r.body.fmRatio       = 2.0f;
    r.body.fmIndex       = (bank == Bank::SouthernFuturism) ? 1.4f : 0.35f;
    r.body.level         = 0.96f;
    r.body.seedOffset    = 11u;

    r.attack.enabled     = true;
    r.attack.genMode     = static_cast<uint8_t>(dsp::ProceduralExciter::Type::HardMallet);
    r.attack.tuneHz      = 1600.0f;
    r.attack.decayMs     = 12.0f;
    r.attack.level       = 0.22f + 0.20f * p.kickClick;
    r.attack.morphOrPos  = 0.70f;
    r.attack.seedOffset  = 29u;

    r.texture.enabled    = true;
    r.texture.genMode    = static_cast<uint8_t>(TextureGenMode::FilteredNoise);
    r.texture.cutoffHz   = 1600.0f + 800.0f * p.grit;
    r.texture.decayMs    = 28.0f;
    r.texture.level      = 0.06f + 0.14f * p.grit;
    r.texture.seedOffset = 53u;
    return r;
}

/** Raw Snare Forge — hard crack + digital grit. */
inline DrumRecipe makeSnare(Bank bank, uint64_t seed = 0xBA115A1EULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r {};
    r.schemaVersion      = 1u;
    r.family             = InstrumentFamily::Snare;
    r.destruction        = (p.grit > 0.45f) ? DestructionTier::Mixed : DestructionTier::Clean;
    r.pitched            = false;
    r.rootHz             = p.snareBodyHz;
    r.masterSeed         = seed;
    r.drive              = 1.8f + 1.4f * p.grit;
    r.fold               = 0.08f + 0.12f * p.grit;
    r.dirtBlend          = 0.30f + 0.40f * p.grit;
    r.samplerGrit12Bit   = p.grit;
    r.shellSize          = 0.22f;
    r.shellCoupling      = 0.28f;
    r.reverbMix          = p.reverb;
    r.stereoWidth        = 0.18f;
    r.outputGain         = 0.88f;

    r.body.enabled       = true;
    r.body.genMode       = static_cast<uint8_t>(BodyGenMode::MembraneModal);
    r.body.tuneHz        = p.snareBodyHz;
    r.body.decayMs       = p.snareDecayMs;
    r.body.morphOrPos    = 0.32f;
    r.body.level         = 0.62f;
    r.body.seedOffset    = 11u;

    r.attack.enabled     = true;
    r.attack.genMode     = static_cast<uint8_t>(dsp::ProceduralExciter::Type::Drumstick);
    r.attack.tuneHz      = 2800.0f + 2200.0f * p.brightness;
    r.attack.decayMs     = 5.0f + 5.0f * p.snareCrack;
    r.attack.morphOrPos  = 0.85f + 0.12f * p.snareCrack;
    r.attack.level       = 0.55f + 0.35f * p.snareCrack;
    r.attack.seedOffset  = 29u;

    r.texture.enabled    = true;
    r.texture.genMode    = static_cast<uint8_t>(TextureGenMode::SnareWireRattle);
    r.texture.cutoffHz   = 4500.0f + 3000.0f * p.brightness;
    r.texture.decayMs    = p.snareDecayMs * (0.7f + 0.5f * p.snareNoise);
    r.texture.level      = 0.50f + 0.35f * p.snareNoise;
    r.texture.seedOffset = 53u;
    return r;
}

inline DrumRecipe makeClap(Bank bank, uint64_t seed = 0xBA11C1A9ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r = makeFactoryRecipe(InstrumentFamily::Clap, cinematic::Mood::Aggressive, seed);
    r.destruction        = DestructionTier::Mixed;
    r.drive              = 2.2f + p.grit;
    r.samplerGrit12Bit   = p.grit;
    r.attack.level       = 0.88f;
    r.texture.cutoffHz   = 2000.0f + 2500.0f * p.brightness;
    r.texture.decayMs    = 55.0f;
    r.reverbMix          = p.reverb;
    r.outputGain         = 0.86f;
    return r;
}

inline DrumRecipe makeClosedHat(Bank bank, uint64_t seed = 0xBA114A70ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r = makeFactoryRecipe(InstrumentFamily::ClosedHat, cinematic::Mood::Aggressive, seed);
    r.destruction        = DestructionTier::Mixed;
    r.drive              = 1.6f + 0.8f * p.grit;
    r.samplerGrit12Bit   = p.grit * 0.7f;
    r.body.decayMs       = 22.0f + 12.0f * (1.0f - p.brightness);
    r.texture.cutoffHz   = 7000.0f + 2000.0f * p.brightness;
    r.texture.level      = 0.70f;
    r.reverbMix          = 0.0f;
    r.outputGain         = 0.70f;
    return r;
}

/** Experimental percussion — FM / metallic / mechanical. */
inline DrumRecipe makeExperimentalPerc(Bank bank, uint64_t seed = 0xBA11E9C0ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r = makeFactoryRecipe(InstrumentFamily::FmBellPerc, cinematic::Mood::Aggressive, seed);
    r.destruction        = DestructionTier::Mixed;
    r.drive              = 2.0f + 1.5f * p.grit;
    r.samplerGrit12Bit   = p.grit;
    r.body.genMode       = static_cast<uint8_t>(BodyGenMode::FmTwoOp);
    r.body.fmIndex       = 1.5f + 2.0f * p.brightness;
    r.body.fmRatio       = 1.7f + 1.3f * p.grit;
    r.body.tuneHz        = 420.0f + 380.0f * p.brightness;
    r.body.decayMs       = 90.0f + 80.0f * (1.0f - p.kickClick);
    r.attack.genMode     = static_cast<uint8_t>(dsp::ProceduralExciter::Type::MetalStick);
    r.attack.level       = 0.70f;
    r.texture.genMode    = static_cast<uint8_t>(TextureGenMode::SixOscMetallic);
    r.texture.level      = 0.55f;
    r.outputGain         = 0.78f;
    return r;
}

inline KitProgram makeKit(Bank bank, uint64_t seed = 0xBA111616ULL) noexcept
{
    KitProgram kit {};
    setBankName(kit, bank);
    const auto p = profileFor(bank);

    kit.pads[0]  = { 36, 0, 1.00f, makeKick(bank, seed + 1) };
    kit.pads[1]  = { 37, 0, 0.90f, makeFactoryRecipe(InstrumentFamily::RimTom, cinematic::Mood::Aggressive, seed + 2) };
    kit.pads[1].recipe.samplerGrit12Bit = p.grit;
    kit.pads[2]  = { 38, 0, 1.00f, makeSnare(bank, seed + 3) };
    kit.pads[3]  = { 39, 0, 0.95f, makeClap(bank, seed + 4) };
    kit.pads[4]  = { 40, 2, 1.00f, makeBass(bank, p.bassRootHz, seed + 5) };
    kit.pads[5]  = { 41, 0, 0.88f, makeExperimentalPerc(bank, seed + 6) };
    kit.pads[6]  = { 42, 1, 0.85f, makeClosedHat(bank, seed + 7) };
    kit.pads[7]  = { 43, 2, 1.00f, makeBass(bank, p.bassRootHz * 0.8908987f, seed + 8) };
    kit.pads[8]  = { 44, 1, 0.80f, makeClosedHat(bank, seed + 9) };
    kit.pads[9]  = { 45, 0, 0.88f, makeExperimentalPerc(bank, seed + 10) };
    kit.pads[10] = { 46, 1, 0.82f, makeFactoryRecipe(InstrumentFamily::OpenHat, cinematic::Mood::Aggressive, seed + 11) };
    kit.pads[10].recipe.samplerGrit12Bit = p.grit * 0.6f;
    kit.pads[11] = { 47, 0, 0.85f, makeExperimentalPerc(bank, seed + 12) };
    kit.pads[12] = { 48, 0, 0.92f, makeKick(bank, seed + 13) };
    kit.pads[13] = { 49, 0, 0.72f, makeFactoryRecipe(InstrumentFamily::CymbalForge, cinematic::Mood::Aggressive, seed + 14) };
    kit.pads[14] = { 50, 0, 0.90f, makeSnare(bank, seed + 15) };
    kit.pads[15] = { 51, 0, 0.75f, makeFactoryRecipe(InstrumentFamily::SyntheticFoleyFx, cinematic::Mood::Aggressive, seed + 16) };
    kit.pads[15].recipe.samplerGrit12Bit = p.grit;

    if (bank == Bank::MinimalMonster)
    {
        // Spare pads 1,5,8,9,11,13,15 for sparse performance kits
        for (int i : { 1, 5, 8, 9, 11, 13, 15 })
            kit.pads[static_cast<size_t>(i)].padGain = 0.55f;
    }
    return kit;
}

} // namespace disklordz::bang
