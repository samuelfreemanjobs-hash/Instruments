#include "FactoryPatchLibrary.h"

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
    elite
};

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
    return PresetCategory::elite;
}

const char* categoryLabel (PresetCategory category) noexcept
{
    switch (category)
    {
        case PresetCategory::epPiano: return "EP";
        case PresetCategory::pad: return "Pad";
        case PresetCategory::bass: return "Bass";
        case PresetCategory::vaporwave: return "Vapor";
        case PresetCategory::rnb80: return "80s R&B";
        case PresetCategory::rnb90: return "90s R&B";
        case PresetCategory::elite: return "Elite";
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
            return (variant % 3 == 0) ? jdupgraded::dsp::ToneCouplingMode::independent
                                        : jdupgraded::dsp::ToneCouplingMode::ringPair01;
        case PresetCategory::epPiano:
            return (variant % 4 == 0) ? jdupgraded::dsp::ToneCouplingMode::crossModPair01
                                      : jdupgraded::dsp::ToneCouplingMode::independent;
        default:
            return jdupgraded::dsp::ToneCouplingMode::independent;
    }
}

FactoryPatch buildPatch (std::size_t index, std::string& nameStorage)
{
    const auto category = categoryForIndex (index);
    const std::size_t variant = index % 24;
    const float v = static_cast<float> (variant);

    char nameBuf[48];
    std::snprintf (nameBuf, sizeof (nameBuf), "%s %02zu", categoryLabel (category), (variant + 1));
    nameStorage = nameBuf;

    FactoryToneLayer t0 {}, t1 {}, t2 {}, t3 {};
    float master = 0.76f;
    float groupA = 0.0f;
    float groupB = 0.12f;

    switch (category)
    {
        case PresetCategory::epPiano:
            t0 = layer (1 + static_cast<std::uint16_t> (variant % 4), 0, 0.92f, 0.0f, 0.88f, 0.38f);
            t1 = layer (0, static_cast<std::uint16_t> (4 + variant), 0.45f, 12.0f + v * 0.5f, 0.75f, 0.32f);
            t2 = layer (0, static_cast<std::uint16_t> (20 + variant % 8), 0.22f, -7.0f, 0.62f, 0.25f);
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

        case PresetCategory::elite:
        default:
            t0 = layer (static_cast<std::uint16_t> (18 + variant % 6), 0, 0.7f, 0.0f, 0.82f, 0.55f);
            t1 = layer (static_cast<std::uint16_t> (20 + variant % 4), 0, 0.65f, -5.0f, 0.75f, 0.52f);
            t2 = layer (0, static_cast<std::uint16_t> (55 + variant % 8), 0.5f, 12.0f, 0.7f, 0.48f);
            t3 = layer (static_cast<std::uint16_t> (11 + variant % 4), 0, 0.4f, -19.0f, 0.5f, 0.45f);
            groupA = 0.15f + v * 0.012f;
            groupB = 0.25f + v * 0.01f;
            master = 0.74f;
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
            nameStorage = "Vapor Stack (curated)";
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

} // namespace jdupgraded::assets
