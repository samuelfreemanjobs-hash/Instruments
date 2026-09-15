#include "FactoryPatchLibrary.h"

#include <vector>

namespace jdupgraded::assets
{

namespace
{

FactoryToneLayer layer (std::uint16_t ms, std::uint16_t wave, float level, float coarse,
                        float cutoff, float resonance)
{
    return FactoryToneLayer { ms, wave, level, coarse, cutoff, resonance };
}

FactoryPatch patch (const char* name,
                    FactoryToneLayer t0, FactoryToneLayer t1, FactoryToneLayer t2, FactoryToneLayer t3,
                    jdupgraded::dsp::ToneCouplingMode coupling,
                    float master, float groupA, float groupB)
{
    return FactoryPatch {
        name,
        { t0, t1, t2, t3 },
        coupling,
        master,
        groupA,
        groupB
    };
}

const std::vector<FactoryPatch>& allPatches()
{
    static const std::vector<FactoryPatch> kPatches = {
        patch ("Init Stack",
               layer (0, 0, 1.0f, 0.0f, 1.0f, 0.3f),
               layer (0, 16, 0.0f, 0.0f, 1.0f, 0.3f),
               layer (0, 32, 0.0f, 0.0f, 1.0f, 0.3f),
               layer (0, 48, 0.0f, 0.0f, 1.0f, 0.3f),
               jdupgraded::dsp::ToneCouplingMode::independent, 0.75f, 0.0f, 0.0f),
        patch ("Analog MS Pad",
               layer (1, 0, 0.85f, 0.0f, 0.82f, 0.42f),
               layer (2, 0, 0.55f, -7.0f, 0.7f, 0.35f),
               layer (0, 8, 0.25f, 12.0f, 0.55f, 0.28f),
               layer (0, 0, 0.0f, 0.0f, 1.0f, 0.3f),
               jdupgraded::dsp::ToneCouplingMode::independent, 0.78f, 0.05f, 0.12f),
        patch ("Digital Glass",
               layer (3, 0, 0.9f, 0.0f, 0.88f, 0.55f),
               layer (4, 0, 0.45f, 5.0f, 0.65f, 0.48f),
               layer (0, 24, 0.2f, 0.0f, 0.5f, 0.2f),
               layer (0, 0, 0.0f, 0.0f, 1.0f, 0.3f),
               jdupgraded::dsp::ToneCouplingMode::crossModPair01, 0.72f, 0.0f, 0.22f),
        patch ("Bell Choir",
               layer (5, 0, 0.8f, 0.0f, 0.75f, 0.62f),
               layer (6, 0, 0.65f, -5.0f, 0.68f, 0.58f),
               layer (0, 40, 0.35f, 7.0f, 0.6f, 0.4f),
               layer (0, 41, 0.2f, -12.0f, 0.55f, 0.35f),
               jdupgraded::dsp::ToneCouplingMode::ringPair23, 0.7f, 0.0f, 0.35f),
        patch ("Pluck Stack",
               layer (7, 0, 0.95f, 0.0f, 0.92f, 0.25f),
               layer (8, 0, 0.5f, 0.0f, 0.8f, 0.22f),
               layer (0, 52, 0.3f, 12.0f, 0.7f, 0.18f),
               layer (0, 0, 0.0f, 0.0f, 1.0f, 0.3f),
               jdupgraded::dsp::ToneCouplingMode::hardSyncPair01, 0.8f, 0.08f, 0.08f),
        patch ("Sub Bass MS",
               layer (9, 0, 1.0f, 0.0f, 0.45f, 0.3f),
               layer (10, 0, 0.35f, 0.0f, 0.55f, 0.28f),
               layer (0, 3, 0.15f, 24.0f, 0.35f, 0.15f),
               layer (0, 0, 0.0f, 0.0f, 1.0f, 0.3f),
               jdupgraded::dsp::ToneCouplingMode::independent, 0.82f, 0.18f, 0.0f),
        patch ("Noise Sweep",
               layer (11, 0, 0.7f, 0.0f, 0.6f, 0.5f),
               layer (0, 55, 0.55f, 0.0f, 0.85f, 0.45f),
               layer (0, 56, 0.4f, -3.0f, 0.75f, 0.4f),
               layer (12, 0, 0.25f, 0.0f, 0.5f, 0.35f),
               jdupgraded::dsp::ToneCouplingMode::crossModPair23, 0.68f, 0.12f, 0.28f),
        patch ("Vocal Hold",
               layer (13, 0, 0.88f, 0.0f, 0.72f, 0.38f),
               layer (14, 0, 0.5f, -2.0f, 0.65f, 0.32f),
               layer (0, 60, 0.22f, 5.0f, 0.58f, 0.25f),
               layer (0, 0, 0.0f, 0.0f, 1.0f, 0.3f),
               jdupgraded::dsp::ToneCouplingMode::independent, 0.74f, 0.0f, 0.18f),
        patch ("Organ Layer",
               layer (15, 0, 0.75f, 0.0f, 0.8f, 0.2f),
               layer (16, 0, 0.75f, 0.0f, 0.78f, 0.2f),
               layer (0, 12, 0.4f, 12.0f, 0.7f, 0.18f),
               layer (0, 13, 0.35f, -12.0f, 0.68f, 0.18f),
               jdupgraded::dsp::ToneCouplingMode::ringPair01, 0.76f, 0.06f, 0.1f),
        patch ("FX Riser",
               layer (17, 0, 0.65f, 0.0f, 0.95f, 0.65f),
               layer (18, 0, 0.55f, 0.0f, 0.88f, 0.6f),
               layer (0, 63, 0.45f, 0.0f, 0.8f, 0.55f),
               layer (19, 0, 0.35f, -5.0f, 0.7f, 0.5f),
               jdupgraded::dsp::ToneCouplingMode::hardSyncPair23, 0.7f, 0.22f, 0.4f),
        patch ("Wide PWM MS",
               layer (20, 0, 0.8f, 0.0f, 0.85f, 0.48f),
               layer (21, 0, 0.6f, 0.0f, 0.75f, 0.42f),
               layer (0, 20, 0.25f, 7.0f, 0.65f, 0.3f),
               layer (0, 21, 0.2f, -7.0f, 0.62f, 0.28f),
               jdupgraded::dsp::ToneCouplingMode::independent, 0.77f, 0.1f, 0.25f),
        patch ("Crystal Split",
               layer (22, 0, 0.7f, 0.0f, 0.9f, 0.52f),
               layer (23, 0, 0.7f, -12.0f, 0.85f, 0.5f),
               layer (24, 0, 0.45f, 12.0f, 0.78f, 0.45f),
               layer (0, 0, 0.0f, 0.0f, 1.0f, 0.3f),
               jdupgraded::dsp::ToneCouplingMode::crossModPair01, 0.73f, 0.0f, 0.3f),
    };
    return kPatches;
}

} // namespace

std::size_t FactoryPatchLibrary::getPatchCount() noexcept
{
    return allPatches().size();
}

const FactoryPatch& FactoryPatchLibrary::getPatch (std::size_t index) noexcept
{
    const auto& patches = allPatches();
    return patches[index % patches.size()];
}

} // namespace jdupgraded::assets
