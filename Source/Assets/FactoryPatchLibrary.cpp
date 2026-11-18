#include "FactoryPatchLibrary.h"

#include "FactoryPatchDedup.h"

#include <cstdio>
#include <string>
#include <vector>

namespace jdupgraded::assets
{

namespace
{

enum class PresetCategory : std::uint8_t
{
    epPiano = 0,
    pad,
    bass,
    vaporwave,
    rnb80,
    rnb90,
    elite,
    trap,
    phonk,
    loFi,
    synthwave,
    house,
    afro,
    jersey,
    ambient,
    mpcPack
};

constexpr std::size_t kLegacyBankEnd = 200;
constexpr std::size_t kExtendedBankSize = 50;

PresetCategory categoryForIndex (std::size_t index) noexcept
{
    if (index < 24)
        return PresetCategory::epPiano;
    if (index < 48)
        return PresetCategory::pad;
    if (index < 64)
        return PresetCategory::bass;
    if (index < 80)
        return PresetCategory::vaporwave;
    if (index < 96)
        return PresetCategory::rnb80;
    if (index < 112)
        return PresetCategory::rnb90;
    if (index < 128)
        return PresetCategory::elite;
    if (index < 152)
        return PresetCategory::trap;
    if (index < 176)
        return PresetCategory::phonk;
    if (index < kLegacyBankEnd)
        return PresetCategory::loFi;
    if (index < 250)
        return PresetCategory::synthwave;
    if (index < 300)
        return PresetCategory::house;
    if (index < 350)
        return PresetCategory::afro;
    if (index < 400)
        return PresetCategory::jersey;
    if (index < 450)
        return PresetCategory::ambient;
    return PresetCategory::mpcPack;
}

std::size_t categoryRangeStart (PresetCategory category) noexcept
{
    switch (category)
    {
        case PresetCategory::epPiano: return 0;
        case PresetCategory::pad: return 24;
        case PresetCategory::bass: return 48;
        case PresetCategory::vaporwave: return 64;
        case PresetCategory::rnb80: return 80;
        case PresetCategory::rnb90: return 96;
        case PresetCategory::elite: return 112;
        case PresetCategory::trap: return 128;
        case PresetCategory::phonk: return 152;
        case PresetCategory::loFi: return 176;
        case PresetCategory::synthwave: return 200;
        case PresetCategory::house: return 250;
        case PresetCategory::afro: return 300;
        case PresetCategory::jersey: return 350;
        case PresetCategory::ambient: return 400;
        case PresetCategory::mpcPack: return 450;
        default: return 0;
    }
}

const char* categoryLabel (PresetCategory category) noexcept
{
    switch (category)
    {
        case PresetCategory::epPiano: return "EP";
        case PresetCategory::pad: return "Pad";
        case PresetCategory::bass: return "Bass";
        case PresetCategory::vaporwave: return "Vaporwave";
        case PresetCategory::rnb80: return "80s R&B";
        case PresetCategory::rnb90: return "90s R&B";
        case PresetCategory::elite: return "Elite";
        case PresetCategory::trap: return "Trap";
        case PresetCategory::phonk: return "Phonk";
        case PresetCategory::loFi: return "Lo-Fi";
        case PresetCategory::synthwave: return "Synthwave";
        case PresetCategory::house: return "House";
        case PresetCategory::afro: return "Afro";
        case PresetCategory::jersey: return "Jersey";
        case PresetCategory::ambient: return "Ambient";
        case PresetCategory::mpcPack: return "MPC Pack";
        default: return "Patch";
    }
}

FactoryToneLayer layer (std::uint16_t ms, std::uint16_t wave, float level, float coarse,
                        float cutoff, float resonance)
{
    return FactoryToneLayer { ms, wave, level, coarse, cutoff, resonance };
}

jdupgraded::dsp::ToneCouplingMode couplingFor (PresetCategory category, std::size_t variant) noexcept
{
    switch (category)
    {
        case PresetCategory::elite:
            switch (variant % 6)
            {
                case 0: return jdupgraded::dsp::ToneCouplingMode::hardSyncPair01;
                case 1: return jdupgraded::dsp::ToneCouplingMode::crossModPair01;
                case 2: return jdupgraded::dsp::ToneCouplingMode::ringPair01;
                case 3: return jdupgraded::dsp::ToneCouplingMode::hardSyncPair23;
                case 4: return jdupgraded::dsp::ToneCouplingMode::crossModPair23;
                default: return jdupgraded::dsp::ToneCouplingMode::ringPair23;
            }
        case PresetCategory::vaporwave:
        case PresetCategory::loFi:
            return (variant % 3 == 0) ? jdupgraded::dsp::ToneCouplingMode::independent
                                        : jdupgraded::dsp::ToneCouplingMode::ringPair01;
        case PresetCategory::trap:
            return (variant % 2 == 0) ? jdupgraded::dsp::ToneCouplingMode::crossModPair01
                                        : jdupgraded::dsp::ToneCouplingMode::independent;
        case PresetCategory::phonk:
            return (variant % 2 == 0) ? jdupgraded::dsp::ToneCouplingMode::ringPair01
                                        : jdupgraded::dsp::ToneCouplingMode::independent;
        case PresetCategory::synthwave:
        case PresetCategory::ambient:
        case PresetCategory::mpcPack:
            return (variant % 3 == 1) ? jdupgraded::dsp::ToneCouplingMode::ringPair01
                                        : jdupgraded::dsp::ToneCouplingMode::independent;
        case PresetCategory::house:
        case PresetCategory::jersey:
            return (variant % 2 == 1) ? jdupgraded::dsp::ToneCouplingMode::crossModPair01
                                        : jdupgraded::dsp::ToneCouplingMode::independent;
        case PresetCategory::afro:
            return (variant % 4 == 0) ? jdupgraded::dsp::ToneCouplingMode::ringPair23
                                        : jdupgraded::dsp::ToneCouplingMode::independent;
        case PresetCategory::epPiano:
            return (variant % 4 == 0) ? jdupgraded::dsp::ToneCouplingMode::crossModPair01
                                      : jdupgraded::dsp::ToneCouplingMode::independent;
        default:
            return jdupgraded::dsp::ToneCouplingMode::independent;
    }
}

void buildExtendedMarketName (std::size_t index, PresetCategory category, std::string& nameStorage)
{
    static constexpr const char* kAdj[6] = { "Neon", "Midnight", "Chrome", "Soft", "Ritual", "Golden" };
    static constexpr const char* kNoun[6] = { "Stack", "Keys", "Pad", "Bass", "Lead", "Bell" };
    const std::size_t slot = index - categoryRangeStart (category);
    const char* bank = categoryLabel (category);
    char buf[72];
    std::snprintf (buf,
                   sizeof (buf),
                   "%s %s %s · P%03zu",
                   kAdj[slot % 6],
                   kNoun[(slot / 3) % 6],
                   bank,
                   index + 1);
    nameStorage = buf;
}

FactoryPatch buildPatch (std::size_t index, std::string& nameStorage)
{
    const auto category = categoryForIndex (index);
    const std::size_t variant = index - categoryRangeStart (category);
    const float v = static_cast<float> (variant);

    if (category == PresetCategory::vaporwave && index >= 64 && index < 80)
    {
        static constexpr const char* kVaporMarketNames[16] = {
            "Palm Mall Keys",
            "Chrome Lobby EP",
            "Slow Pacific Pad",
            "VHS Sunset Stack",
            "Pink Atrium Bell",
            "Escalator Dreams",
            "Aqua Tile Rhodes",
            "Midnight Food Court",
            "Marble Fountain Pad",
            "Sundown Fax Tone",
            "Soft Channel Surf",
            "Crt Glow Keys",
            "Ritual Elevator Pad",
            "Limewire Haze",
            "Ocean View Mall",
            "Velvet Escalator",
        };
        nameStorage = kVaporMarketNames[index - 64];
    }
    else if (category == PresetCategory::trap && index >= 128 && index < 152)
    {
        static constexpr const char* kTrapMarketNames[24] = {
            "808 Cart Push", "Rack Room Sub", "Gunshot Pluck", "Night 808", "Slime Lead",
            "Metro Bell", "Opium Pad", "Atlanta Keys", "Shadow 808", "Vault Sub",
            "Dark Hall Pluck", "Chrome 808", "Drill Stack", "Dusty Bell", "Hood Pad",
            "Witch Lead", "Trap EP 01", "Low Fog Bass", "Ice Pluck", "Saint Bell",
            "Smoke Pad", "Ritual Lead", "Zone 808", "Backroom Sub",
        };
        nameStorage = kTrapMarketNames[index - 128];
    }
    else if (category == PresetCategory::phonk && index >= 152 && index < 176)
    {
        static constexpr const char* kPhonkMarketNames[24] = {
            "Memphis Cow Route", "Graveyard Bell", "Chopped Organ", "Swamp Lead", "Dust Tape Keys",
            "Ritual Cow", "Phonk Stab 01", "Vintage Horn", "Grim Bell", "Night Organ",
            "Gravel Lead", "Hollow Cow", "Funeral Stab", "Delta Keys", "Smoke Organ",
            "Rust Bell", "Backroad Lead", "Crypt Cow", "Ash Stab", "Field Organ",
            "Bone Lead", "Memphis Night", "Dirt Cow", "Lantern Keys",
        };
        nameStorage = kPhonkMarketNames[index - 152];
    }
    else if (category == PresetCategory::loFi && index >= 176 && index < 200)
    {
        static constexpr const char* kLoFiMarketNames[24] = {
            "Rain Window EP", "Dusk Mall Pad", "Study Rhodes", "Tape Haze Keys", "Loft Sunset",
            "Coffee Pad", "Soft Channel", "Letter Rhodes", "Evening Mall", "Warm Static Pad",
            "Balcony Keys", "Late Train EP", "Cloud Pad", "Bedroom Rhodes", "City Glow",
            "Polaroid Pad", "Quiet Hall EP", "Dream Transit", "Sodium Pad", "Home Rhodes",
            "Blue Hour", "Lobby Lo-Fi", "Sleep Mall", "Golden Escalator",
        };
        nameStorage = kLoFiMarketNames[index - 176];
    }
    else if (index >= kLegacyBankEnd)
    {
        buildExtendedMarketName (index, category, nameStorage);
    }
    else
    {
        char nameBuf[56];
        std::snprintf (nameBuf,
                       sizeof (nameBuf),
                       "%s %02zu · P%03zu",
                       categoryLabel (category),
                       variant + 1,
                       index + 1);
        nameStorage = nameBuf;
    }

    FactoryToneLayer t0 {}, t1 {}, t2 {}, t3 {};
    float master = 0.76f;
    float groupA = 0.0f;
    float groupB = 0.12f;

    switch (category)
    {
        case PresetCategory::epPiano:
            t0 = layer (1 + static_cast<std::uint16_t> (variant % 4), 0, 0.92f, 0.0f, 0.88f, 0.38f);
            t1 = layer (0, static_cast<std::uint16_t> (4 + variant + (index % 11)), 0.45f, 12.0f + v * 0.5f, 0.75f, 0.32f);
            t2 = layer (0, static_cast<std::uint16_t> (20 + (variant + index) % 8), 0.22f, -7.0f, 0.62f, 0.25f);
            t3 = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.3f);
            groupB = 0.15f + v * 0.005f;
            break;

        case PresetCategory::pad:
            t0 = layer (static_cast<std::uint16_t> (13 + variant % 6), 0, 0.75f, 0.0f, 0.7f, 0.28f);
            t1 = layer (static_cast<std::uint16_t> (15 + variant % 5), 0, 0.62f, -5.0f - v * 0.2f, 0.65f, 0.25f);
            t2 = layer (0, static_cast<std::uint16_t> (40 + variant % 12), 0.38f, 7.0f, 0.58f, 0.22f);
            t3 = layer (0, static_cast<std::uint16_t> (50 + variant % 10), 0.25f, -12.0f, 0.52f, 0.2f);
            groupB = 0.28f + v * 0.008f;
            break;

        case PresetCategory::bass:
            t0 = layer (static_cast<std::uint16_t> (9 + variant % 4), 0, 1.0f, 0.0f, 0.42f, 0.3f);
            t1 = layer (static_cast<std::uint16_t> (10 + variant % 3), 0, 0.4f, 0.0f, 0.55f, 0.28f);
            t2 = layer (0, static_cast<std::uint16_t> (3 + variant % 6), 0.2f, 24.0f, 0.35f, 0.18f);
            t3 = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.3f);
            groupA = 0.12f + v * 0.01f;
            break;

        case PresetCategory::vaporwave:
            t0 = layer (1, 0, 0.8f, -14.0f - v * 0.3f, 0.72f, 0.3f);
            t1 = layer (2, 0, 0.72f, 14.0f + v * 0.25f, 0.68f, 0.28f);
            t2 = layer (static_cast<std::uint16_t> (17 + variant % 4), 0, 0.45f, 0.0f, 0.55f, 0.22f);
            t3 = layer (0, static_cast<std::uint16_t> (60 + variant % 8), 0.3f, 5.0f, 0.48f, 0.2f);
            groupB = 0.38f + v * 0.01f;
            groupA = 0.05f;
            break;

        case PresetCategory::rnb80:
            t0 = layer (3, 0, 0.88f, 0.0f, 0.85f, 0.42f);
            t1 = layer (5, 0, 0.5f, 5.0f, 0.7f, 0.38f);
            t2 = layer (static_cast<std::uint16_t> (15 + variant % 4), 0, 0.35f, -3.0f, 0.62f, 0.25f);
            t3 = layer (static_cast<std::uint16_t> (9 + variant % 3), 0, 0.55f, -12.0f, 0.4f, 0.28f);
            groupB = 0.22f;
            break;

        case PresetCategory::rnb90:
            t0 = layer (2, 0, 0.86f, 0.0f, 0.78f, 0.35f);
            t1 = layer (static_cast<std::uint16_t> (13 + variant % 5), 0, 0.58f, -2.0f, 0.65f, 0.3f);
            t2 = layer (static_cast<std::uint16_t> (6 + variant % 4), 0, 0.42f, 7.0f, 0.68f, 0.32f);
            t3 = layer (static_cast<std::uint16_t> (10 + variant % 3), 0, 0.65f, 0.0f, 0.38f, 0.25f);
            groupB = 0.3f;
            groupA = 0.08f;
            break;

        case PresetCategory::trap:
            t0 = layer (static_cast<std::uint16_t> (8 + variant % 4), 0, 1.0f, -12.0f, 0.38f, 0.35f);
            t1 = layer (static_cast<std::uint16_t> (10 + variant % 5), 0, 0.55f, 0.0f, 0.48f, 0.32f);
            t2 = layer (0, static_cast<std::uint16_t> (12 + variant % 8), 0.35f, 12.0f, 0.52f, 0.28f);
            t3 = layer (0, static_cast<std::uint16_t> (28 + variant % 6), 0.25f, 7.0f, 0.45f, 0.25f);
            groupA = 0.14f + v * 0.008f;
            groupB = 0.18f;
            master = 0.8f;
            break;

        case PresetCategory::phonk:
            t0 = layer (static_cast<std::uint16_t> (11 + variant % 4), 0, 0.92f, -7.0f, 0.55f, 0.42f);
            t1 = layer (0, static_cast<std::uint16_t> (44 + variant % 10), 0.48f, 5.0f, 0.62f, 0.38f);
            t2 = layer (static_cast<std::uint16_t> (6 + variant % 3), 0, 0.42f, 0.0f, 0.5f, 0.35f);
            t3 = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.3f);
            groupA = 0.1f;
            groupB = 0.22f;
            break;

        case PresetCategory::loFi:
            t0 = layer (1, 0, 0.78f, -8.0f - v * 0.2f, 0.7f, 0.26f);
            t1 = layer (2, 0, 0.65f, 7.0f + v * 0.15f, 0.66f, 0.24f);
            t2 = layer (static_cast<std::uint16_t> (14 + variant % 5), 0, 0.4f, 0.0f, 0.58f, 0.22f);
            t3 = layer (0, static_cast<std::uint16_t> (72 + variant % 8), 0.28f, 3.0f, 0.5f, 0.2f);
            groupB = 0.34f;
            groupA = 0.04f;
            break;

        case PresetCategory::synthwave:
            t0 = layer (static_cast<std::uint16_t> (4 + (index % 6)), 0, 0.82f, -5.0f - v * 0.1f, 0.76f, 0.32f);
            t1 = layer (0, static_cast<std::uint16_t> (32 + (index % 20)), 0.58f, 5.0f, 0.72f, 0.28f);
            t2 = layer (static_cast<std::uint16_t> (16 + variant % 6), 0, 0.38f, 0.0f, 0.6f, 0.24f);
            t3 = layer (0, static_cast<std::uint16_t> (80 + (index % 16)), 0.22f, 12.0f, 0.52f, 0.22f);
            groupB = 0.3f + v * 0.004f;
            break;

        case PresetCategory::house:
            t0 = layer (static_cast<std::uint16_t> (7 + variant % 5), 0, 0.9f, 0.0f, 0.68f, 0.35f);
            t1 = layer (0, static_cast<std::uint16_t> (24 + (index % 24)), 0.52f, 7.0f, 0.62f, 0.3f);
            t2 = layer (static_cast<std::uint16_t> (12 + variant % 4), 0, 0.45f, -3.0f, 0.58f, 0.28f);
            t3 = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.2f);
            groupB = 0.26f;
            groupA = 0.06f;
            break;

        case PresetCategory::afro:
            t0 = layer (static_cast<std::uint16_t> (5 + variant % 4), 0, 0.88f, 0.0f, 0.62f, 0.36f);
            t1 = layer (0, static_cast<std::uint16_t> (40 + (index % 18)), 0.55f, 5.0f, 0.58f, 0.32f);
            t2 = layer (static_cast<std::uint16_t> (8 + variant % 3), 0, 0.5f, -7.0f, 0.55f, 0.3f);
            t3 = layer (static_cast<std::uint16_t> (10 + variant % 3), 0, 0.62f, 12.0f, 0.42f, 0.28f);
            groupB = 0.24f;
            break;

        case PresetCategory::jersey:
            t0 = layer (static_cast<std::uint16_t> (9 + variant % 3), 0, 1.0f, -12.0f, 0.44f, 0.38f);
            t1 = layer (0, static_cast<std::uint16_t> (16 + (index % 12)), 0.48f, 0.0f, 0.5f, 0.34f);
            t2 = layer (static_cast<std::uint16_t> (6 + variant % 4), 0, 0.4f, 7.0f, 0.48f, 0.3f);
            t3 = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.2f);
            groupA = 0.12f;
            master = 0.82f;
            break;

        case PresetCategory::ambient:
            t0 = layer (static_cast<std::uint16_t> (14 + variant % 5), 0, 0.72f, 0.0f, 0.78f, 0.22f);
            t1 = layer (0, static_cast<std::uint16_t> (56 + (index % 24)), 0.55f, -2.0f, 0.7f, 0.2f);
            t2 = layer (static_cast<std::uint16_t> (18 + variant % 4), 0, 0.42f, 5.0f, 0.62f, 0.18f);
            t3 = layer (0, static_cast<std::uint16_t> (96 + (index % 12)), 0.25f, -5.0f, 0.55f, 0.16f);
            groupB = 0.36f;
            break;

        case PresetCategory::mpcPack:
            t0 = layer (static_cast<std::uint16_t> (2 + variant % 4), 0, 1.0f, -12.0f, 0.4f, 0.34f);
            t1 = layer (static_cast<std::uint16_t> (10 + variant % 3), 0, 0.58f, 0.0f, 0.46f, 0.3f);
            t2 = layer (0, static_cast<std::uint16_t> (8 + (index % 20)), 0.35f, 12.0f, 0.5f, 0.26f);
            t3 = layer (0, static_cast<std::uint16_t> (28 + (index % 14)), 0.2f, 7.0f, 0.44f, 0.22f);
            groupA = 0.1f + v * 0.006f;
            groupB = 0.16f;
            master = 0.84f;
            break;

        case PresetCategory::elite:
            t0 = layer (static_cast<std::uint16_t> (18 + variant % 6), 0, 0.7f, 0.0f, 0.82f, 0.55f);
            t1 = layer (static_cast<std::uint16_t> (20 + variant % 4), 0, 0.65f, -5.0f, 0.75f, 0.52f);
            t2 = layer (0, static_cast<std::uint16_t> (55 + variant % 8), 0.5f, 12.0f, 0.7f, 0.48f);
            t3 = layer (static_cast<std::uint16_t> (11 + variant % 4), 0, 0.4f, -19.0f, 0.5f, 0.45f);
            groupA = 0.15f + v * 0.012f;
            groupB = 0.25f + v * 0.01f;
            master = 0.74f;
            break;

        default:
            break;
    }

    return FactoryPatch {
        nameStorage.c_str(),
        { t0, t1, t2, t3 },
        couplingFor (category, variant),
        master,
        groupA,
        groupB
    };
}

void applyCuratedOverrides (std::size_t index, FactoryPatch& patch, std::string& nameStorage)
{
    switch (index)
    {
        case 0:
            nameStorage = "EP Glass 01 (curated)";
            patch.tones[0] = layer (0, 8, 1.0f, 0.0f, 0.92f, 0.22f);
            patch.tones[1] = layer (0, 24, 0.35f, 0.07f, 0.88f, 0.18f);
            patch.tones[2] = layer (0, 40, 0.2f, -0.05f, 0.75f, 0.12f);
            patch.tones[3] = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.1f);
            patch.groupBMix = 0.18f;
            break;
        case 48:
            nameStorage = "Bass Sub 01 (curated)";
            patch.tones[0] = layer (2, 4, 1.0f, -12.0f, 0.55f, 0.45f);
            patch.tones[1] = layer (2, 12, 0.55f, -12.0f, 0.48f, 0.4f);
            patch.tones[2] = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.2f);
            patch.tones[3] = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.2f);
            patch.masterGain = 0.82f;
            patch.groupADrive = 0.08f;
            break;
        case 24:
            nameStorage = "Pad Glass 01 (curated)";
            patch.tones[0] = layer (1, 64, 0.85f, 0.0f, 0.95f, 0.2f);
            patch.tones[1] = layer (1, 80, 0.6f, 0.12f, 0.9f, 0.25f);
            patch.tones[2] = layer (1, 96, 0.45f, -0.08f, 0.85f, 0.18f);
            patch.tones[3] = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.1f);
            patch.groupBMix = 0.32f;
            break;
        case 64:
            nameStorage = "Palm Mall Stack (curated)";
            patch.tones[0] = layer (3, 128, 0.75f, 0.0f, 0.8f, 0.3f);
            patch.tones[1] = layer (3, 144, 0.7f, 0.05f, 0.78f, 0.28f);
            patch.tones[2] = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.2f);
            patch.tones[3] = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.2f);
            patch.coupling = jdupgraded::dsp::ToneCouplingMode::ringPair01;
            patch.groupBMix = 0.4f;
            break;
        case 80:
            nameStorage = "80s R&B Keys (curated)";
            patch.tones[0] = layer (5, 32, 0.9f, 0.0f, 0.88f, 0.3f);
            patch.tones[1] = layer (5, 48, 0.55f, 0.1f, 0.82f, 0.28f);
            patch.tones[2] = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.2f);
            patch.tones[3] = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.2f);
            patch.groupADrive = 0.05f;
            patch.groupBMix = 0.28f;
            break;
        case 128:
            nameStorage = "808 Cart Push (curated)";
            patch.tones[0] = layer (2, 8, 1.0f, -12.0f, 0.36f, 0.4f);
            patch.tones[1] = layer (2, 16, 0.5f, -12.0f, 0.42f, 0.35f);
            patch.coupling = jdupgraded::dsp::ToneCouplingMode::crossModPair01;
            patch.groupADrive = 0.16f;
            break;
        case 152:
            nameStorage = "Memphis Cow Route (curated)";
            patch.tones[0] = layer (3, 52, 0.95f, -5.0f, 0.58f, 0.45f);
            patch.tones[1] = layer (0, 48, 0.5f, 7.0f, 0.55f, 0.38f);
            patch.coupling = jdupgraded::dsp::ToneCouplingMode::ringPair01;
            break;
        case 176:
            nameStorage = "Rain Window EP (curated)";
            patch.tones[0] = layer (1, 72, 0.85f, -10.0f, 0.72f, 0.28f);
            patch.tones[1] = layer (2, 88, 0.62f, 8.0f, 0.68f, 0.26f);
            patch.groupBMix = 0.36f;
            break;
        case 200:
            nameStorage = "Sunset Grid Stack (curated)";
            patch.tones[0] = layer (4, 40, 0.88f, 0.0f, 0.78f, 0.34f);
            patch.tones[1] = layer (0, 64, 0.55f, 7.0f, 0.72f, 0.3f);
            patch.groupBMix = 0.32f;
            break;
        case 300:
            nameStorage = "Afro Gold Stack (curated)";
            patch.tones[0] = layer (6, 24, 0.92f, 0.0f, 0.58f, 0.38f);
            patch.groupBMix = 0.28f;
            break;
        case 450:
            nameStorage = "MPC Cart Classics (curated)";
            patch.tones[0] = layer (2, 6, 1.0f, -12.0f, 0.38f, 0.36f);
            patch.groupADrive = 0.14f;
            break;
        case 112:
            nameStorage = "Elite Sync Lead (curated)";
            patch.tones[0] = layer (4, 96, 0.9f, 0.0f, 0.7f, 0.55f);
            patch.tones[1] = layer (4, 112, 0.85f, 0.0f, 0.65f, 0.5f);
            patch.tones[2] = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.2f);
            patch.tones[3] = layer (0, 0, 0.0f, 0.0f, 1.0f, 0.2f);
            patch.coupling = jdupgraded::dsp::ToneCouplingMode::hardSyncPair01;
            patch.groupADrive = 0.22f;
            patch.groupBMix = 0.2f;
            break;
        default:
            break;
    }
}

struct PatchStore final
{
    std::vector<std::string> names;
    std::vector<FactoryPatch> patches;
};

const PatchStore& patchStore()
{
    static PatchStore store = []() -> PatchStore
    {
        PatchStore result;
        result.names.reserve (kFactoryPatchCount);
        result.patches.reserve (kFactoryPatchCount);

        result.names.resize (kFactoryPatchCount);
        result.patches.resize (kFactoryPatchCount);
        for (std::size_t i = 0; i < kFactoryPatchCount; ++i)
        {
            result.patches[i] = buildPatch (i, result.names[i]);
            applyCuratedOverrides (i, result.patches[i], result.names[i]);
            result.patches[i].name = result.names[i].c_str();
        }
        detail::deduplicatePatchStore (result.names, result.patches);
        return result;
    }();
    return store;
}

} // namespace

std::size_t FactoryPatchLibrary::getPatchCount() noexcept
{
    return kFactoryPatchCount;
}

const FactoryPatch& FactoryPatchLibrary::getPatch (std::size_t index) noexcept
{
    const auto& store = patchStore();
    return store.patches[index % store.patches.size()];
}

const char* FactoryPatchLibrary::getCategoryName (std::size_t index) noexcept
{
    return categoryLabel (categoryForIndex (index));
}

} // namespace jdupgraded::assets
