#pragma once
// DiskLordz Southern Triumph / Drum Squad Forge — 8 DNA banks + Triple Kick Architect.
#include "../ThreeLayerDrum.h"
#include <algorithm>
#include <array>
#include <cstring>

namespace disklordz::southern
{

enum class Bank : uint8_t
{
    SouthernTriumph = 0,
    MemphisRoots,
    AtlantaAnthem,
    OrchestralSoul,
    TripleKickWarfare,
    DrumSquadClassics,
    BluesAndTrap,
    EpicStreetCinema,
    Count
};

inline constexpr std::array<const char*, 8> kBankNames {{
    "Southern Triumph",
    "Memphis Roots",
    "Atlanta Anthem",
    "Orchestral Soul",
    "Triple Kick Warfare",
    "Drum Squad Classics",
    "Blues & Trap",
    "Epic Street Cinema"
}};

struct BankProfile
{
    float kickFundHz = 50.f;
    float kickStartHz = 190.f;
    float kickSweepMs = 14.f;
    float kickDecayMs = 160.f;
    float punchHz = 95.f;
    float punchLevel = 0.55f;
    float clickLevel = 0.65f;
    float kickDrive = 2.4f;
    float bassRootHz = 46.25f;
    float bassStartMul = 3.0f;
    float bassDecayMs = 520.f;
    float bassDrive = 2.2f;
    float snareBodyHz = 195.f;
    float snareDecayMs = 90.f;
    float snareCrack = 0.75f;
    float snareNoise = 0.60f;
    float grit = 0.35f;
    float brightness = 0.65f;
    float reverb = 0.06f;
    float orchestral = 0.20f;
};

inline constexpr std::array<BankProfile, 8> kProfiles {{
    { 50.f, 200.f, 14.f, 180.f, 98.f, 0.60f, 0.70f, 2.6f, 46.25f, 3.1f, 560.f, 2.4f, 200.f, 95.f, 0.80f, 0.55f, 0.30f, 0.70f, 0.06f, 0.35f },
    { 52.f, 210.f, 12.f, 140.f, 105.f, 0.50f, 0.75f, 2.9f, 43.65f, 3.3f, 480.f, 2.7f, 210.f, 75.f, 0.85f, 0.65f, 0.65f, 0.72f, 0.03f, 0.10f },
    { 54.f, 220.f, 10.f, 130.f, 110.f, 0.65f, 0.82f, 2.8f, 49.f, 3.2f, 450.f, 2.5f, 205.f, 70.f, 0.88f, 0.50f, 0.25f, 0.80f, 0.04f, 0.15f },
    { 44.f, 150.f, 22.f, 280.f, 80.f, 0.70f, 0.45f, 1.8f, 41.2f, 2.4f, 700.f, 1.6f, 180.f, 140.f, 0.55f, 0.70f, 0.08f, 0.50f, 0.18f, 0.85f },
    { 48.f, 230.f, 8.f, 110.f, 120.f, 0.80f, 0.90f, 3.0f, 46.25f, 3.5f, 400.f, 2.8f, 215.f, 65.f, 0.90f, 0.45f, 0.40f, 0.78f, 0.02f, 0.20f },
    { 55.f, 205.f, 13.f, 150.f, 100.f, 0.55f, 0.72f, 2.7f, 49.f, 3.0f, 500.f, 2.5f, 198.f, 80.f, 0.82f, 0.58f, 0.55f, 0.70f, 0.04f, 0.12f },
    { 50.f, 160.f, 20.f, 200.f, 88.f, 0.45f, 0.50f, 1.9f, 48.999f, 2.5f, 580.f, 1.8f, 175.f, 120.f, 0.60f, 0.72f, 0.15f, 0.48f, 0.10f, 0.40f },
    { 42.f, 175.f, 18.f, 320.f, 85.f, 0.75f, 0.60f, 2.5f, 36.71f, 2.8f, 780.f, 2.3f, 185.f, 160.f, 0.70f, 0.65f, 0.20f, 0.55f, 0.22f, 0.95f }
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

/** Three-component kick: SUB + PUNCH + CLICK via body/attack/texture. */
inline DrumRecipe makeTripleKick(Bank bank, uint64_t seed = 0x57100001ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r {};
    r.schemaVersion = 1u;
    r.family = InstrumentFamily::Kick;
    r.destruction = (p.grit > 0.5f) ? DestructionTier::Mixed : DestructionTier::Clean;
    r.pitched = false;
    r.rootHz = p.kickFundHz;
    r.masterSeed = seed;
    r.drive = p.kickDrive;
    r.fold = 0.05f + 0.08f * p.grit;
    r.dirtBlend = 0.30f + 0.35f * p.clickLevel;
    r.samplerGrit12Bit = p.grit;
    r.reverbMix = p.reverb * (0.3f + 0.7f * p.orchestral);
    r.roomSize = 0.35f + 0.40f * p.orchestral;
    r.outputGain = 0.90f;

    r.body.enabled = true;
    r.body.genMode = (p.orchestral > 0.6f)
                         ? static_cast<uint8_t>(BodyGenMode::MembraneModal)
                         : static_cast<uint8_t>(BodyGenMode::AnalogSineTri);
    r.body.tuneHz = p.kickFundHz;
    r.body.pitchStart = std::max(p.kickStartHz, p.punchHz * 1.6f);
    r.body.pitchMs = p.kickSweepMs;
    r.body.decayMs = p.kickDecayMs;
    r.body.morphOrPos = 0.15f + 0.45f * p.punchLevel;
    r.body.level = 0.95f;
    r.body.cutoffHz = 4500.f;
    r.body.resonance = 0.7f + 0.3f * p.punchLevel;
    r.body.seedOffset = 11u;

    r.attack.enabled = true;
    r.attack.genMode = static_cast<uint8_t>(dsp::ProceduralExciter::Type::HardMallet);
    r.attack.tuneHz = 2000.f + 1600.f * p.brightness;
    r.attack.decayMs = 5.f + 10.f * p.clickLevel;
    r.attack.morphOrPos = 0.70f + 0.25f * p.clickLevel;
    r.attack.level = 0.35f + 0.50f * p.clickLevel;
    r.attack.seedOffset = 29u;

    r.texture.enabled = true;
    r.texture.genMode = static_cast<uint8_t>(TextureGenMode::FilteredNoise);
    r.texture.cutoffHz = 2200.f + 1600.f * p.brightness;
    r.texture.decayMs = 20.f + 40.f * p.orchestral;
    r.texture.level = 0.12f + 0.20f * p.grit + 0.15f * p.orchestral;
    r.texture.seedOffset = 53u;
    return r;
}

inline DrumRecipe makeKick(Bank bank, uint64_t seed = 0x57100002ULL) noexcept
{
    return makeTripleKick(bank, seed);
}

inline DrumRecipe makeBass(Bank bank, float rootHz = 0.f, uint64_t seed = 0x57100808ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r {};
    r.schemaVersion = 1u;
    r.family = InstrumentFamily::Bass808;
    r.destruction = (p.grit > 0.5f) ? DestructionTier::Mixed : DestructionTier::Clean;
    r.pitched = true;
    r.rootHz = (rootHz > 20.f) ? rootHz : p.bassRootHz;
    r.masterSeed = seed;
    r.glideMs = (bank == Bank::AtlantaAnthem || bank == Bank::TripleKickWarfare) ? 35.f : 10.f;
    r.drive = p.bassDrive;
    r.fold = 0.04f + 0.10f * p.grit;
    r.dirtBlend = 0.20f + 0.35f * p.grit;
    r.samplerGrit12Bit = p.grit * 0.8f;
    r.subCrossoverHz = 110.f;
    r.reverbMix = 0.f;
    r.outputGain = 0.92f;

    r.body.enabled = true;
    r.body.genMode = static_cast<uint8_t>(BodyGenMode::AnalogSineTri);
    r.body.tuneHz = r.rootHz;
    r.body.pitchStart = r.rootHz * p.bassStartMul;
    r.body.pitchMs = 24.f;
    r.body.decayMs = p.bassDecayMs;
    r.body.morphOrPos = 0.22f + 0.15f * p.grit;
    r.body.level = 0.96f;
    r.body.seedOffset = 11u;

    r.attack.enabled = true;
    r.attack.genMode = static_cast<uint8_t>(dsp::ProceduralExciter::Type::HardMallet);
    r.attack.tuneHz = 1500.f;
    r.attack.decayMs = 14.f;
    r.attack.level = 0.20f + 0.25f * p.clickLevel;
    r.attack.morphOrPos = 0.68f;
    r.attack.seedOffset = 29u;

    r.texture.enabled = true;
    r.texture.genMode = static_cast<uint8_t>(TextureGenMode::FilteredNoise);
    r.texture.cutoffHz = 1500.f + 900.f * p.grit;
    r.texture.decayMs = 30.f;
    r.texture.level = 0.06f + 0.12f * p.grit;
    r.texture.seedOffset = 53u;
    return r;
}

inline DrumRecipe makeSnare(Bank bank, uint64_t seed = 0x57105A1EULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r {};
    r.schemaVersion = 1u;
    r.family = (p.orchestral > 0.7f) ? InstrumentFamily::MilitarySnareForge : InstrumentFamily::Snare;
    r.destruction = (p.grit > 0.45f) ? DestructionTier::Mixed : DestructionTier::Clean;
    r.pitched = false;
    r.rootHz = p.snareBodyHz;
    r.masterSeed = seed;
    r.drive = 1.6f + 1.2f * p.grit;
    r.fold = 0.06f + 0.10f * p.grit;
    r.dirtBlend = 0.25f + 0.35f * p.grit;
    r.samplerGrit12Bit = p.grit;
    r.shellSize = 0.25f + 0.35f * p.orchestral;
    r.shellCoupling = 0.30f + 0.20f * p.orchestral;
    r.reverbMix = p.reverb;
    r.roomSize = 0.30f + 0.45f * p.orchestral;
    r.stereoWidth = 0.20f;
    r.outputGain = 0.88f;

    r.body.enabled = true;
    r.body.genMode = static_cast<uint8_t>(BodyGenMode::MembraneModal);
    r.body.tuneHz = p.snareBodyHz;
    r.body.decayMs = p.snareDecayMs;
    r.body.morphOrPos = 0.35f;
    r.body.level = 0.70f;
    r.body.seedOffset = 11u;

    r.attack.enabled = true;
    r.attack.genMode = static_cast<uint8_t>(dsp::ProceduralExciter::Type::Drumstick);
    r.attack.tuneHz = 2600.f + 2200.f * p.brightness;
    r.attack.decayMs = 6.f + 6.f * p.snareCrack;
    r.attack.morphOrPos = 0.80f + 0.15f * p.snareCrack;
    r.attack.level = 0.50f + 0.40f * p.snareCrack;
    r.attack.seedOffset = 29u;

    r.texture.enabled = true;
    r.texture.genMode = static_cast<uint8_t>(TextureGenMode::SnareWireRattle);
    r.texture.cutoffHz = 4000.f + 2800.f * p.brightness;
    r.texture.decayMs = p.snareDecayMs * (0.8f + 0.5f * p.snareNoise);
    r.texture.level = 0.55f + 0.30f * p.snareNoise;
    r.texture.seedOffset = 53u;
    return r;
}

inline DrumRecipe makeClap(Bank bank, uint64_t seed = 0x5710C1A9ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r = makeFactoryRecipe(InstrumentFamily::Clap, cinematic::Mood::Aggressive, seed);
    r.destruction = DestructionTier::Mixed;
    r.drive = 2.0f + p.grit;
    r.samplerGrit12Bit = p.grit;
    r.reverbMix = p.reverb;
    r.attack.level = 0.85f;
    r.texture.cutoffHz = 1800.f + 2400.f * p.brightness;
    r.outputGain = 0.86f;
    return r;
}

inline DrumRecipe makeClosedHat(Bank bank, uint64_t seed = 0x57104A70ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r = makeFactoryRecipe(InstrumentFamily::ClosedHat, cinematic::Mood::Aggressive, seed);
    r.drive = 1.5f + 0.7f * p.grit;
    r.samplerGrit12Bit = p.grit * 0.6f;
    r.body.decayMs = 24.f + 14.f * (1.f - p.brightness);
    r.texture.cutoffHz = 6500.f + 2500.f * p.brightness;
    r.outputGain = 0.72f;
    return r;
}

inline KitProgram makeKit(Bank bank, uint64_t seed = 0x57101616ULL) noexcept
{
    KitProgram kit {};
    setBankName(kit, bank);
    const auto p = profileFor(bank);

    auto kickSub = makeTripleKick(bank, seed + 1);
    kickSub.attack.level *= 0.55f;
    kickSub.body.morphOrPos = 0.12f;

    auto kickPunch = makeTripleKick(bank, seed + 2);
    kickPunch.body.tuneHz = p.punchHz;
    kickPunch.body.pitchStart = p.punchHz * 2.2f;
    kickPunch.body.decayMs = p.kickDecayMs * 0.65f;
    kickPunch.attack.level *= 0.85f;

    auto kickClick = makeTripleKick(bank, seed + 3);
    kickClick.body.level = 0.55f;
    kickClick.attack.level = std::min(1.2f, kickClick.attack.level * 1.35f);
    kickClick.texture.level *= 1.25f;

    kit.pads[0] = { 36, 0, 1.00f, kickSub };
    kit.pads[1] = { 37, 0, 0.95f, kickPunch };
    kit.pads[2] = { 38, 0, 0.90f, makeSnare(bank, seed + 4) };
    kit.pads[3] = { 39, 0, 0.95f, makeClap(bank, seed + 5) };
    kit.pads[4] = { 40, 2, 1.00f, makeBass(bank, p.bassRootHz, seed + 6) };
    kit.pads[5] = { 41, 0, 0.95f, kickClick };
    kit.pads[6] = { 42, 1, 0.85f, makeClosedHat(bank, seed + 7) };
    kit.pads[7] = { 43, 2, 1.00f, makeBass(bank, p.bassRootHz * 0.8908987f, seed + 8) };
    kit.pads[8] = { 44, 1, 0.80f, makeClosedHat(bank, seed + 9) };
    kit.pads[9] = { 45, 0, 0.92f, makeFactoryRecipe(InstrumentFamily::TimpaniForge, cinematic::Mood::Luxury, seed + 10) };
    kit.pads[10] = { 46, 1, 0.82f, makeFactoryRecipe(InstrumentFamily::OpenHat, cinematic::Mood::Aggressive, seed + 11) };
    kit.pads[11] = { 47, 0, 0.90f, makeFactoryRecipe(InstrumentFamily::WarDrumForge, cinematic::Mood::Aggressive, seed + 12) };
    kit.pads[12] = { 48, 0, 0.95f, makeTripleKick(bank, seed + 13) };
    kit.pads[13] = { 49, 0, 0.78f, makeFactoryRecipe(InstrumentFamily::CymbalForge, cinematic::Mood::Luxury, seed + 14) };
    kit.pads[14] = { 50, 0, 0.92f, makeFactoryRecipe(InstrumentFamily::MilitarySnareForge, cinematic::Mood::Aggressive, seed + 15) };
    kit.pads[15] = { 51, 0, 0.85f, makeFactoryRecipe(InstrumentFamily::CinematicImpactForge, cinematic::Mood::Aggressive, seed + 16) };

    if (bank == Bank::OrchestralSoul || bank == Bank::EpicStreetCinema)
    {
        kit.pads[9].padGain = 1.0f;
        kit.pads[11].padGain = 1.0f;
        kit.pads[15].padGain = 1.0f;
        kit.pads[15].recipe.reverbMix = 0.28f;
    }
    if (bank == Bank::BluesAndTrap)
    {
        kit.pads[0].recipe.drive = 1.6f;
        kit.pads[2].recipe.drive = 1.4f;
        kit.pads[2].recipe.samplerGrit12Bit = 0.08f;
    }
    return kit;
}

} // namespace disklordz::southern
