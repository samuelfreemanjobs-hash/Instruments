#pragma once
// DiskLordz Hitmaker Drum Lab — 8 DNA banks + Multi-808 Fusion kit wiring.
#include "../DSP/Multi808FusionReactor.h"
#include "../ThreeLayerDrum.h"
#include <array>
#include <cstring>

namespace disklordz::hitmaker
{

enum class Bank : uint8_t
{
    Earthquake = 0,
    AtlantaHeat,
    TrunkRattle,
    HitRecord,
    DarkExperiment,
    ClubAnthem,
    MelodicPressure,
    FutureMachine,
    Count
};

inline constexpr std::array<const char*, 8> kBankNames {{
    "Earthquake",
    "Atlanta Heat",
    "Trunk Rattle",
    "Hit Record",
    "Dark Experiment",
    "Club Anthem",
    "Melodic Pressure",
    "Future Machine"
}};

struct BankProfile
{
    int fusionFamily = 0;
    float kickFundHz = 52.f;
    float kickStartHz = 200.f;
    float kickSweepMs = 14.f;
    float kickDecayMs = 140.f;
    float kickClick = 0.7f;
    float kickDrive = 2.6f;
    float snareBodyHz = 210.f;
    float snareDecayMs = 90.f;
    float snareCrack = 0.8f;
    float snareNoise = 0.55f;
    float grit = 0.25f;
    float brightness = 0.72f;
    float reverb = 0.04f;
};

inline constexpr std::array<BankProfile, 8> kProfiles {{
    { 0, 48.f, 220.f, 12.f, 150.f, 0.85f, 3.0f, 220.f, 85.f, 0.88f, 0.50f, 0.35f, 0.75f, 0.03f }, // Earthquake
    { 3, 52.f, 205.f, 14.f, 130.f, 0.75f, 2.8f, 215.f, 80.f, 0.85f, 0.58f, 0.40f, 0.78f, 0.03f }, // Atlanta Heat
    { 1, 46.f, 180.f, 18.f, 160.f, 0.65f, 2.5f, 200.f, 95.f, 0.78f, 0.50f, 0.30f, 0.65f, 0.05f }, // Trunk Rattle
    { 8, 54.f, 190.f, 12.f, 120.f, 0.70f, 2.2f, 205.f, 75.f, 0.82f, 0.45f, 0.10f, 0.80f, 0.04f }, // Hit Record
    { 6, 44.f, 170.f, 20.f, 170.f, 0.55f, 2.4f, 180.f, 120.f, 0.70f, 0.70f, 0.45f, 0.55f, 0.08f }, // Dark Experiment
    { 5, 50.f, 230.f, 10.f, 110.f, 0.88f, 2.9f, 230.f, 70.f, 0.90f, 0.48f, 0.20f, 0.85f, 0.04f }, // Club Anthem
    { 13, 50.f, 160.f, 18.f, 155.f, 0.50f, 1.9f, 190.f, 110.f, 0.65f, 0.50f, 0.08f, 0.62f, 0.07f }, // Melodic Pressure
    { 11, 48.f, 200.f, 16.f, 145.f, 0.60f, 2.5f, 210.f, 100.f, 0.75f, 0.60f, 0.35f, 0.78f, 0.05f }  // Future Machine
}};

inline BankProfile profileFor(Bank bank) noexcept
{
    return kProfiles[static_cast<size_t>(bank) % 8u];
}

inline FusionParams fusionFor(Bank bank) noexcept
{
    const auto bp = profileFor(bank);
    return makeFusionFamily(bp.fusionFamily);
}

inline void setBankName(KitProgram& kit, Bank bank) noexcept
{
    const char* name = kBankNames[static_cast<size_t>(bank) % 8u];
    std::memset(kit.bankName.data(), 0, kit.bankName.size());
    for (size_t i = 0; i + 1 < kit.bankName.size() && name[i] != '\0'; ++i)
        kit.bankName[i] = name[i];
}

inline DrumRecipe makeKick(Bank bank, uint64_t seed = 0x41700001ULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r {};
    r.schemaVersion = 1u;
    r.family = InstrumentFamily::Kick;
    r.destruction = (p.grit > 0.3f) ? DestructionTier::Mixed : DestructionTier::Clean;
    r.pitched = false;
    r.rootHz = p.kickFundHz;
    r.masterSeed = seed;
    r.drive = p.kickDrive;
    r.fold = 0.05f + 0.1f * p.grit;
    r.dirtBlend = 0.3f + 0.4f * p.kickClick;
    r.samplerGrit12Bit = p.grit;
    r.reverbMix = p.reverb;
    r.outputGain = 0.90f;

    r.body.enabled = true;
    r.body.genMode = static_cast<uint8_t>(BodyGenMode::AnalogSineTri);
    r.body.tuneHz = p.kickFundHz;
    r.body.pitchStart = p.kickStartHz;
    r.body.pitchMs = p.kickSweepMs;
    r.body.decayMs = p.kickDecayMs;
    r.body.morphOrPos = 0.2f + 0.25f * p.kickClick;
    r.body.level = 0.95f;
    r.body.seedOffset = 11u;

    r.attack.enabled = true;
    r.attack.genMode = static_cast<uint8_t>(dsp::ProceduralExciter::Type::HardMallet);
    r.attack.tuneHz = 2000.f + 1500.f * p.brightness;
    r.attack.decayMs = 5.f + 8.f * p.kickClick;
    r.attack.morphOrPos = 0.75f + 0.2f * p.kickClick;
    r.attack.level = 0.4f + 0.45f * p.kickClick;
    r.attack.seedOffset = 29u;

    r.texture.enabled = true;
    r.texture.genMode = static_cast<uint8_t>(TextureGenMode::FilteredNoise);
    r.texture.cutoffHz = 2600.f + 1600.f * p.brightness;
    r.texture.decayMs = 18.f;
    r.texture.level = 0.1f + 0.15f * p.grit;
    r.texture.seedOffset = 53u;
    return r;
}

/** Map fusion params into a Bass808 three-layer recipe (compat with DrumVoiceManager). */
inline DrumRecipe makeBassRecipe(Bank bank, float rootHz = 0.f, uint64_t seed = 0x41700808ULL) noexcept
{
    const auto f = fusionFor(bank);
    const auto p = profileFor(bank);
    DrumRecipe r {};
    r.schemaVersion = 1u;
    r.family = InstrumentFamily::Bass808;
    r.destruction = (f.dirtLevel > 0.5f) ? DestructionTier::Mixed : DestructionTier::Clean;
    r.pitched = true;
    r.rootHz = (rootHz > 20.f) ? rootHz : f.rootHz;
    r.masterSeed = seed;
    r.glideMs = f.glideMs;
    r.drive = f.gritDrive * 0.55f;
    r.fold = f.wavefold;
    r.dirtBlend = f.dirtLevel;
    r.subCrossoverHz = f.crossoverHz;
    r.samplerGrit12Bit = p.grit * 0.5f;
    r.reverbMix = 0.f;
    r.outputGain = f.outputGain;

    r.body.enabled = true;
    r.body.genMode = static_cast<uint8_t>(BodyGenMode::AnalogSineTri);
    r.body.tuneHz = r.rootHz;
    r.body.pitchStart = f.punchStartHz;
    r.body.pitchMs = f.punchSweepMs;
    r.body.decayMs = f.subDecayMs;
    r.body.morphOrPos = f.sineTriMorph;
    r.body.level = f.cleanLevel;
    r.body.seedOffset = 11u;

    r.attack.enabled = true;
    r.attack.genMode = static_cast<uint8_t>(dsp::ProceduralExciter::Type::HardMallet);
    r.attack.tuneHz = f.punchStartHz * 1.2f;
    r.attack.decayMs = f.punchDecayMs * 0.15f;
    r.attack.level = f.punchLevel;
    r.attack.morphOrPos = 0.75f;
    r.attack.seedOffset = 29u;

    r.texture.enabled = true;
    r.texture.genMode = static_cast<uint8_t>(TextureGenMode::FilteredNoise);
    r.texture.cutoffHz = 1800.f + 1200.f * f.dirtLevel;
    r.texture.decayMs = f.dirtDecayMs * 0.25f;
    r.texture.level = f.dirtLevel * 0.35f;
    r.texture.seedOffset = 53u;
    return r;
}

inline DrumRecipe makeSnare(Bank bank, uint64_t seed = 0x41705A1EULL) noexcept
{
    const auto p = profileFor(bank);
    DrumRecipe r {};
    r.schemaVersion = 1u;
    r.family = InstrumentFamily::Snare;
    r.destruction = (p.grit > 0.3f) ? DestructionTier::Mixed : DestructionTier::Clean;
    r.pitched = false;
    r.rootHz = p.snareBodyHz;
    r.masterSeed = seed;
    r.drive = 1.7f + 1.2f * p.grit;
    r.fold = 0.06f + 0.1f * p.grit;
    r.dirtBlend = 0.25f + 0.35f * p.grit;
    r.samplerGrit12Bit = p.grit;
    r.shellSize = 0.25f;
    r.shellCoupling = 0.3f;
    r.reverbMix = p.reverb;
    r.outputGain = 0.88f;

    r.body.enabled = true;
    r.body.genMode = static_cast<uint8_t>(BodyGenMode::MembraneModal);
    r.body.tuneHz = p.snareBodyHz;
    r.body.decayMs = p.snareDecayMs;
    r.body.level = 0.68f;
    r.body.seedOffset = 11u;

    r.attack.enabled = true;
    r.attack.genMode = static_cast<uint8_t>(dsp::ProceduralExciter::Type::Drumstick);
    r.attack.tuneHz = 2800.f + 2200.f * p.brightness;
    r.attack.decayMs = 6.f + 5.f * p.snareCrack;
    r.attack.morphOrPos = 0.82f;
    r.attack.level = 0.5f + 0.4f * p.snareCrack;
    r.attack.seedOffset = 29u;

    r.texture.enabled = true;
    r.texture.genMode = static_cast<uint8_t>(TextureGenMode::SnareWireRattle);
    r.texture.cutoffHz = 4200.f + 2800.f * p.brightness;
    r.texture.decayMs = p.snareDecayMs * (0.8f + 0.4f * p.snareNoise);
    r.texture.level = 0.55f + 0.3f * p.snareNoise;
    r.texture.seedOffset = 53u;
    return r;
}

inline KitProgram makeKit(Bank bank, uint64_t seed = 0x41701616ULL) noexcept
{
    KitProgram kit {};
    setBankName(kit, bank);
    const auto f = fusionFor(bank);

    kit.pads[0] = { 36, 0, 1.00f, makeKick(bank, seed + 1) };
    kit.pads[1] = { 37, 0, 0.90f, makeFactoryRecipe(InstrumentFamily::RimTom, cinematic::Mood::Aggressive, seed + 2) };
    kit.pads[2] = { 38, 0, 1.00f, makeSnare(bank, seed + 3) };
    kit.pads[3] = { 39, 0, 0.95f, makeFactoryRecipe(InstrumentFamily::Clap, cinematic::Mood::Aggressive, seed + 4) };
    kit.pads[4] = { 40, 2, 1.00f, makeBassRecipe(bank, f.rootHz, seed + 5) };
    kit.pads[5] = { 41, 0, 0.90f, makeFactoryRecipe(InstrumentFamily::FmBellPerc, cinematic::Mood::Aggressive, seed + 6) };
    kit.pads[6] = { 42, 1, 0.85f, makeFactoryRecipe(InstrumentFamily::ClosedHat, cinematic::Mood::Aggressive, seed + 7) };
    kit.pads[7] = { 43, 2, 1.00f, makeBassRecipe(bank, f.rootHz * 0.8909f, seed + 8) };
    kit.pads[8] = { 44, 1, 0.80f, makeFactoryRecipe(InstrumentFamily::ClosedHat, cinematic::Mood::Aggressive, seed + 9) };
    kit.pads[9] = { 45, 0, 0.88f, makeFactoryRecipe(InstrumentFamily::TomForge, cinematic::Mood::Aggressive, seed + 10) };
    kit.pads[10] = { 46, 1, 0.82f, makeFactoryRecipe(InstrumentFamily::OpenHat, cinematic::Mood::Aggressive, seed + 11) };
    kit.pads[11] = { 47, 0, 0.85f, makeFactoryRecipe(InstrumentFamily::FmBellPerc, cinematic::Mood::Aggressive, seed + 12) };
    kit.pads[12] = { 48, 0, 0.95f, makeKick(bank, seed + 13) };
    kit.pads[13] = { 49, 0, 0.75f, makeFactoryRecipe(InstrumentFamily::CymbalForge, cinematic::Mood::Aggressive, seed + 14) };
    kit.pads[14] = { 50, 0, 0.90f, makeSnare(bank, seed + 15) };
    kit.pads[15] = { 51, 0, 0.80f, makeFactoryRecipe(InstrumentFamily::SyntheticFoleyFx, cinematic::Mood::Aggressive, seed + 16) };

    // Fusion-aware dirt on experimental / future banks
    if (bank == Bank::DarkExperiment || bank == Bank::FutureMachine)
    {
        kit.pads[5].recipe.drive = 3.5f;
        kit.pads[11].recipe.drive = 3.2f;
        kit.pads[15].recipe.samplerGrit12Bit = 0.55f;
    }
    return kit;
}

} // namespace disklordz::hitmaker
