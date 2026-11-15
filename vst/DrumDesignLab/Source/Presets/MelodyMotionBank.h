#pragma once
// DiskLordz Melody & Motion — London-influenced melodic Atlanta trap recipes.
#include "../ThreeLayerDrum.h"
#include <algorithm>
#include <array>
#include <cstring>

namespace disklordz::melodymotion
{

enum class Bank : uint8_t
{
    RichBounce = 0,
    SlimeMelody,
    ChurchToTrap,
    AtlantaLifestyle,
    KeysAndBells,
    StreetAnthem,
    MelodicLuxury,
    LateNightLondon,
    Count
};

inline constexpr std::array<const char*, 8> kBankNames {{
    "Rich Bounce",
    "Slime Melody",
    "Church to Trap",
    "Atlanta Lifestyle",
    "Keys & Bells",
    "Street Anthem",
    "Melodic Luxury",
    "Late Night London"
}};

struct BankProfile
{
    float kickFundHz = 52.f;
    float kickStartHz = 170.f;
    float kickSweepMs = 16.f;
    float kickDecayMs = 150.f;
    float kickClick = 0.55f;
    float kickDrive = 2.0f;
    float bassRootHz = 49.f;
    float bassStartMul = 2.8f;
    float bassDecayMs = 560.f;
    float bassMorph = 0.22f;
    float bassDrive = 1.9f;
    float glideMs = 40.f;
    float snareBodyHz = 195.f;
    float snareDecayMs = 95.f;
    float snareCrack = 0.72f;
    float snareNoise = 0.55f;
    float grit = 0.15f;
    float brightness = 0.68f;
    float reverb = 0.06f;
    float bellHz = 880.f;
    float bellDecayMs = 420.f;
    float fmIndex = 1.2f;
};

inline constexpr std::array<BankProfile, 8> kProfiles {{
    // Rich Bounce
    { 52.f, 175.f, 14.f, 145.f, 0.58f, 2.1f, 49.f, 2.9f, 540.f, 0.24f, 2.0f, 35.f, 200.f, 90.f, 0.75f, 0.52f, 0.12f, 0.72f, 0.05f, 920.f, 380.f, 1.1f },
    // Slime Melody — elastic slides, sparse
    { 48.f, 155.f, 22.f, 160.f, 0.42f, 1.7f, 46.25f, 3.2f, 680.f, 0.30f, 1.8f, 85.f, 185.f, 110.f, 0.62f, 0.48f, 0.10f, 0.60f, 0.07f, 740.f, 520.f, 1.6f },
    // Church to Trap
    { 50.f, 160.f, 18.f, 170.f, 0.50f, 1.8f, 48.999f, 2.5f, 500.f, 0.18f, 1.6f, 25.f, 178.f, 125.f, 0.58f, 0.65f, 0.08f, 0.55f, 0.10f, 660.f, 600.f, 0.9f },
    // Atlanta Lifestyle
    { 54.f, 180.f, 12.f, 140.f, 0.60f, 2.2f, 51.91f, 2.7f, 520.f, 0.20f, 1.95f, 30.f, 205.f, 85.f, 0.78f, 0.50f, 0.14f, 0.74f, 0.04f, 980.f, 350.f, 1.3f },
    // Keys & Bells
    { 50.f, 150.f, 20.f, 155.f, 0.45f, 1.6f, 49.f, 2.4f, 480.f, 0.15f, 1.5f, 45.f, 190.f, 100.f, 0.65f, 0.45f, 0.06f, 0.70f, 0.08f, 1046.5f, 720.f, 2.2f },
    // Street Anthem
    { 52.f, 210.f, 10.f, 125.f, 0.78f, 2.7f, 43.65f, 3.3f, 450.f, 0.28f, 2.5f, 20.f, 215.f, 70.f, 0.88f, 0.55f, 0.28f, 0.80f, 0.03f, 880.f, 280.f, 1.0f },
    // Melodic Luxury
    { 50.f, 145.f, 18.f, 175.f, 0.48f, 1.7f, 55.f, 2.3f, 620.f, 0.16f, 1.55f, 50.f, 185.f, 115.f, 0.60f, 0.50f, 0.05f, 0.62f, 0.09f, 784.f, 550.f, 1.4f },
    // Late Night London
    { 46.f, 130.f, 24.f, 190.f, 0.38f, 1.5f, 41.2f, 2.6f, 700.f, 0.12f, 1.4f, 60.f, 170.f, 130.f, 0.52f, 0.42f, 0.18f, 0.45f, 0.12f, 587.f, 480.f, 1.1f }
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

inline DrumRecipe makeKick(Bank bank, uint64_t seed = 0x10D00001ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r {};
    r.schemaVersion = 1u;
    r.family = InstrumentFamily::Kick;
    r.destruction = DestructionTier::Clean;
    r.pitched = false;
    r.rootHz = p.kickFundHz;
    r.masterSeed = seed;
    r.drive = p.kickDrive;
    r.fold = 0.04f;
    r.dirtBlend = 0.22f + 0.25f * p.kickClick;
    r.samplerGrit12Bit = p.grit * 0.4f;
    r.reverbMix = p.reverb * 0.4f;
    r.outputGain = 0.88f;

    r.body.enabled = true;
    r.body.genMode = static_cast<uint8_t>(BodyGenMode::AnalogSineTri);
    r.body.tuneHz = p.kickFundHz;
    r.body.pitchStart = p.kickStartHz;
    r.body.pitchMs = p.kickSweepMs;
    r.body.decayMs = p.kickDecayMs;
    r.body.morphOrPos = 0.18f + 0.15f * p.kickClick;
    r.body.level = 0.94f;
    r.body.seedOffset = 11u;

    r.attack.enabled = true;
    r.attack.genMode = static_cast<uint8_t>(dsp::ProceduralExciter::Type::FeltBeater);
    r.attack.tuneHz = 1800.f + 1000.f * p.brightness;
    r.attack.decayMs = 8.f + 8.f * p.kickClick;
    r.attack.morphOrPos = 0.60f + 0.25f * p.kickClick;
    r.attack.level = 0.35f + 0.30f * p.kickClick;
    r.attack.seedOffset = 29u;

    r.texture.enabled = true;
    r.texture.genMode = static_cast<uint8_t>(TextureGenMode::FilteredNoise);
    r.texture.cutoffHz = 2400.f + 1200.f * p.brightness;
    r.texture.decayMs = 22.f;
    r.texture.level = 0.10f + 0.10f * p.grit;
    r.texture.seedOffset = 53u;
    return r;
}

inline DrumRecipe makeBass(Bank bank, float rootHz = 0.f, uint64_t seed = 0x10D00808ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r {};
    r.schemaVersion = 1u;
    r.family = InstrumentFamily::Bass808;
    r.destruction = DestructionTier::Clean;
    r.pitched = true;
    r.rootHz = (rootHz > 20.f) ? rootHz : p.bassRootHz;
    r.masterSeed = seed;
    r.glideMs = p.glideMs;
    r.drive = p.bassDrive;
    r.fold = 0.03f;
    r.dirtBlend = 0.15f + 0.15f * p.grit;
    r.samplerGrit12Bit = p.grit * 0.3f;
    r.subCrossoverHz = 110.f;
    r.reverbMix = 0.f;
    r.outputGain = 0.92f;

    r.body.enabled = true;
    r.body.genMode = static_cast<uint8_t>(BodyGenMode::AnalogSineTri);
    r.body.tuneHz = r.rootHz;
    r.body.pitchStart = r.rootHz * p.bassStartMul;
    r.body.pitchMs = 26.f;
    r.body.decayMs = p.bassDecayMs;
    r.body.morphOrPos = p.bassMorph;
    r.body.level = 0.95f;
    r.body.seedOffset = 11u;

    r.attack.enabled = true;
    r.attack.genMode = static_cast<uint8_t>(dsp::ProceduralExciter::Type::HardMallet);
    r.attack.tuneHz = 1400.f;
    r.attack.decayMs = 14.f;
    r.attack.level = 0.18f + 0.15f * p.kickClick;
    r.attack.morphOrPos = 0.55f;
    r.attack.seedOffset = 29u;

    r.texture.enabled = true;
    r.texture.genMode = static_cast<uint8_t>(TextureGenMode::FilteredNoise);
    r.texture.cutoffHz = 1600.f;
    r.texture.decayMs = 30.f;
    r.texture.level = 0.06f + 0.08f * p.grit;
    r.texture.seedOffset = 53u;
    return r;
}

inline DrumRecipe makeSnare(Bank bank, uint64_t seed = 0x10D05A1EULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r {};
    r.schemaVersion = 1u;
    r.family = InstrumentFamily::Snare;
    r.destruction = DestructionTier::Clean;
    r.pitched = false;
    r.rootHz = p.snareBodyHz;
    r.masterSeed = seed;
    r.drive = 1.4f + 0.6f * p.grit;
    r.fold = 0.05f;
    r.dirtBlend = 0.18f + 0.20f * p.grit;
    r.samplerGrit12Bit = p.grit;
    r.shellSize = 0.28f;
    r.shellCoupling = 0.32f;
    r.reverbMix = p.reverb;
    r.stereoWidth = 0.22f;
    r.outputGain = 0.86f;

    r.body.enabled = true;
    r.body.genMode = static_cast<uint8_t>(BodyGenMode::MembraneModal);
    r.body.tuneHz = p.snareBodyHz;
    r.body.decayMs = p.snareDecayMs;
    r.body.morphOrPos = 0.36f;
    r.body.level = 0.70f;
    r.body.seedOffset = 11u;

    r.attack.enabled = true;
    r.attack.genMode = static_cast<uint8_t>(dsp::ProceduralExciter::Type::Drumstick);
    r.attack.tuneHz = 2800.f + 1800.f * p.brightness;
    r.attack.decayMs = 7.f + 5.f * p.snareCrack;
    r.attack.morphOrPos = 0.75f + 0.15f * p.snareCrack;
    r.attack.level = 0.45f + 0.35f * p.snareCrack;
    r.attack.seedOffset = 29u;

    r.texture.enabled = true;
    r.texture.genMode = static_cast<uint8_t>(TextureGenMode::SnareWireRattle);
    r.texture.cutoffHz = 4000.f + 2500.f * p.brightness;
    r.texture.decayMs = p.snareDecayMs * (0.85f + 0.4f * p.snareNoise);
    r.texture.level = 0.55f + 0.28f * p.snareNoise;
    r.texture.seedOffset = 53u;
    return r;
}

/** Keys & Bells — FM / modal chromatic percussion. */
inline DrumRecipe makeBell(Bank bank, float noteHz = 0.f, uint64_t seed = 0x10D0BE11ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r {};
    r.schemaVersion = 1u;
    r.family = InstrumentFamily::FmBellPerc;
    r.destruction = DestructionTier::Clean;
    r.pitched = true;
    r.rootHz = (noteHz > 40.f) ? noteHz : p.bellHz;
    r.masterSeed = seed;
    r.drive = 1.3f;
    r.fold = 0.08f + 0.10f * (p.fmIndex / 2.5f);
    r.dirtBlend = 0.12f;
    r.reverbMix = 0.08f + 0.10f * p.reverb;
    r.stereoWidth = 0.35f;
    r.outputGain = 0.78f;

    r.body.enabled = true;
    r.body.genMode = static_cast<uint8_t>(BodyGenMode::FmTwoOp);
    r.body.tuneHz = r.rootHz;
    r.body.pitchStart = r.rootHz * 1.08f;
    r.body.pitchMs = 12.f;
    r.body.decayMs = p.bellDecayMs;
    r.body.fmRatio = 3.5f;
    r.body.fmIndex = p.fmIndex;
    r.body.morphOrPos = 0.35f;
    r.body.level = 0.88f;
    r.body.seedOffset = 11u;

    r.attack.enabled = true;
    r.attack.genMode = static_cast<uint8_t>(dsp::ProceduralExciter::Type::MetalStick);
    r.attack.tuneHz = r.rootHz * 2.5f;
    r.attack.decayMs = 10.f;
    r.attack.morphOrPos = 0.80f;
    r.attack.level = 0.55f;
    r.attack.seedOffset = 29u;

    r.texture.enabled = true;
    r.texture.genMode = static_cast<uint8_t>(TextureGenMode::SixOscMetallic);
    r.texture.tuneHz = r.rootHz * 1.5f;
    r.texture.cutoffHz = 6000.f + 3000.f * p.brightness;
    r.texture.decayMs = p.bellDecayMs * 0.7f;
    r.texture.level = 0.40f + 0.20f * p.brightness;
    r.texture.seedOffset = 53u;
    return r;
}

inline DrumRecipe makeClap(Bank bank, uint64_t seed = 0x10D0C1A9ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r = makeFactoryRecipe(InstrumentFamily::Clap, cinematic::Mood::Luxury, seed);
    r.destruction = DestructionTier::Clean;
    r.drive = 1.5f + 0.4f * p.grit;
    r.reverbMix = p.reverb;
    r.attack.level = 0.82f;
    r.texture.cutoffHz = 1700.f + 2200.f * p.brightness;
    r.outputGain = 0.84f;
    return r;
}

inline DrumRecipe makeClosedHat(Bank bank, uint64_t seed = 0x10D04A70ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r = makeFactoryRecipe(InstrumentFamily::ClosedHat, cinematic::Mood::Luxury, seed);
    r.destruction = DestructionTier::Clean;
    r.drive = 1.3f;
    r.body.decayMs = 28.f + 16.f * (1.f - p.brightness);
    r.texture.cutoffHz = 6000.f + 4000.f * p.brightness;
    r.texture.level = 0.65f;
    // Late Night / Luxury: darker hats
    if (bank == Bank::LateNightLondon || bank == Bank::MelodicLuxury)
        r.texture.cutoffHz = 4500.f + 1500.f * p.brightness;
    r.outputGain = 0.70f;
    return r;
}

inline KitProgram makeKit(Bank bank, uint64_t seed = 0x10D01616ULL) noexcept
{
    KitProgram kit {};
    setBankName(kit, bank);
    const auto p = profileFor(bank);

    kit.pads[0] = { 36, 0, 1.00f, makeKick(bank, seed + 1) };
    kit.pads[1] = { 37, 0, 0.90f, makeFactoryRecipe(InstrumentFamily::RimTom, cinematic::Mood::Luxury, seed + 2) };
    kit.pads[2] = { 38, 0, 1.00f, makeSnare(bank, seed + 3) };
    kit.pads[3] = { 39, 0, 0.95f, makeClap(bank, seed + 4) };
    kit.pads[4] = { 40, 2, 1.00f, makeBass(bank, p.bassRootHz, seed + 5) };
    kit.pads[5] = { 41, 0, 0.88f, makeBell(bank, p.bellHz * 0.5f, seed + 6) };
    kit.pads[6] = { 42, 1, 0.85f, makeClosedHat(bank, seed + 7) };
    kit.pads[7] = { 43, 2, 1.00f, makeBass(bank, p.bassRootHz * 1.122462f /* +2st */, seed + 8) };
    kit.pads[8] = { 44, 1, 0.80f, makeClosedHat(bank, seed + 9) };
    kit.pads[9] = { 45, 0, 0.90f, makeBell(bank, p.bellHz, seed + 10) };
    kit.pads[10] = { 46, 1, 0.82f, makeFactoryRecipe(InstrumentFamily::OpenHat, cinematic::Mood::Luxury, seed + 11) };
    kit.pads[11] = { 47, 0, 0.88f, makeBell(bank, p.bellHz * 1.3348f, seed + 12) };
    kit.pads[12] = { 48, 0, 0.92f, makeKick(bank, seed + 13) };
    kit.pads[13] = { 49, 0, 0.75f, makeFactoryRecipe(InstrumentFamily::CymbalForge, cinematic::Mood::Luxury, seed + 14) };
    kit.pads[14] = { 50, 0, 0.90f, makeSnare(bank, seed + 15) };
    kit.pads[15] = { 51, 0, 0.80f, makeBell(bank, p.bellHz * 2.0f, seed + 16) };

    if (bank == Bank::KeysAndBells)
    {
        kit.pads[5].padGain = 1.0f;
        kit.pads[9].padGain = 1.0f;
        kit.pads[11].padGain = 1.0f;
        kit.pads[15].padGain = 1.0f;
    }
    if (bank == Bank::SlimeMelody)
    {
        kit.pads[4].recipe.glideMs = 90.f;
        kit.pads[7].recipe.glideMs = 90.f;
    }
    return kit;
}

} // namespace disklordz::melodymotion
