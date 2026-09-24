#include "Presets/PresetFactory.h"

#include "Presets/FoundationPresets.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace prophetrev2::presets
{

namespace
{

uint32_t seedFor (int foundationIndex, int variationIndex) noexcept
{
    return static_cast<uint32_t> (foundationIndex * 1009u + variationIndex * 7919u + 0x0C1Cu);
}

float randSigned (uint32_t& state) noexcept
{
    state = state * 1664525u + 1013904223u;
    return (static_cast<float> (state >> 8) / 16777216.0f) * 2.0f - 1.0f;
}

void clampParams (SynthParams& p) noexcept
{
    auto c = [] (float& v, float lo, float hi) { v = std::clamp (v, lo, hi); };
    c (p.osc1Level, 0.0f, 1.0f);
    c (p.osc2Level, 0.0f, 1.0f);
    c (p.osc2DetuneCents, -50.0f, 50.0f);
    c (p.oscMix, 0.0f, 1.0f);
    c (p.filterCutoff, 0.05f, 0.95f);
    c (p.filterRes, 0.0f, 1.0f);
    c (p.filtEnvAmt, 0.0f, 1.0f);
    c (p.keyTrack, 0.0f, 1.0f);
    c (p.circuitDrive, 0.0f, 1.0f);
    c (p.filterDrive, 0.0f, 1.0f);
    c (p.unisonSpread, 0.0f, 40.0f);
    c (p.glideMs, 0.0f, 500.0f);
    c (p.ampAttack, 0.001f, 3.0f);
    c (p.ampDecay, 0.001f, 3.0f);
    c (p.ampSustain, 0.0f, 1.0f);
    c (p.ampRelease, 0.001f, 5.0f);
    c (p.filtAttack, 0.001f, 3.0f);
    c (p.filtDecay, 0.001f, 3.0f);
    c (p.filtSustain, 0.0f, 1.0f);
    c (p.filtRelease, 0.001f, 5.0f);
    p.unisonVoices = static_cast<float> (std::clamp (static_cast<int> (p.unisonVoices + 0.5f), 1, 3));
}

SynthParams applyVariation (const SynthParams& foundation, int foundationIndex, int variationIndex) noexcept
{
    SynthParams p = foundation;
    uint32_t state = seedFor (foundationIndex, variationIndex);
    const float spread = 0.08f + 0.04f * static_cast<float> (variationIndex % 12);

    p.filterCutoff += randSigned (state) * spread;
    p.filterRes += randSigned (state) * (spread * 0.6f);
    p.osc2DetuneCents += randSigned (state) * (spread * 120.0f);
    p.oscMix += randSigned (state) * (spread * 0.5f);
    p.filtEnvAmt += randSigned (state) * (spread * 0.7f);
    p.circuitDrive += randSigned (state) * (spread * 0.35f);
    p.filterDrive += randSigned (state) * (spread * 0.35f);
    p.ampAttack *= 1.0f + randSigned (state) * (spread * 0.5f);
    p.ampDecay *= 1.0f + randSigned (state) * (spread * 0.5f);
    p.ampRelease *= 1.0f + randSigned (state) * (spread * 0.4f);
    p.filtDecay *= 1.0f + randSigned (state) * (spread * 0.45f);

    if ((variationIndex % 5) == 0)
        p.unisonVoices = static_cast<float> (1 + (variationIndex / 5) % 3);

    clampParams (p);
    return p;
}

int variationsForFoundation (int foundationIndex) noexcept
{
    // 20 foundations × 48 + 1 foundation × 47 = 1007
    return foundationIndex < 20 ? 48 : 47;
}

std::string variationName (std::string_view foundationName, int variationNumber) noexcept
{
    std::ostringstream oss;
    oss << foundationName << " · V" << std::setw (3) << std::setfill ('0') << variationNumber;
    return oss.str();
}

} // namespace

std::vector<FactoryPreset> PresetFactory::buildLibrary()
{
    const auto& foundations = getFoundationPresets();
    std::vector<FactoryPreset> library;
    library.reserve (static_cast<std::size_t> (kFactoryPresetCount));

    for (int f = 0; f < kFoundationPresetCount; ++f)
    {
        const auto& src = foundations[static_cast<std::size_t> (f)];
        FactoryPreset foundation;
        foundation.name = std::string (src.name);
        foundation.category = std::string (src.category);
        foundation.params = src.params;
        foundation.foundationIndex = f;
        foundation.isFoundation = true;
        library.push_back (std::move (foundation));
    }

    for (int f = 0; f < kFoundationPresetCount; ++f)
    {
        const auto& src = foundations[static_cast<std::size_t> (f)];
        const int varCount = variationsForFoundation (f);
        for (int v = 1; v <= varCount; ++v)
        {
            FactoryPreset preset;
            preset.name = variationName (src.name, v);
            preset.category = std::string (src.category);
            preset.params = applyVariation (src.params, f, v);
            preset.foundationIndex = f;
            preset.isFoundation = false;
            library.push_back (std::move (preset));
        }
    }

    return library;
}

} // namespace prophetrev2::presets
