#include "Presets/PresetFactory.h"

#include "Presets/FoundationPresets.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <unordered_set>
#include <vector>

namespace prophetrev2::presets
{

namespace
{

struct CategoryQuota
{
    std::string_view category;
    int count;
};

constexpr CategoryQuota kQuotas[] = {
    { "Bass", 195 },
    { "Synth", 245 },
    { "Lead", 196 },
    { "Pad", 196 },
    { "Pluck/Keys", 196 },
};

static_assert (std::size (kQuotas) == 5, "five factory categories");

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
    // Use by-value lambda (not auto&) — temporaries must not bind to non-const lvalue ref.
    auto clampOne = [] (float& v, float lo, float hi) { v = std::clamp (v, lo, hi); };
    clampOne (p.osc1Level, 0.0f, 1.0f);
    clampOne (p.osc2Level, 0.0f, 1.0f);
    clampOne (p.osc2DetuneCents, -50.0f, 50.0f);
    clampOne (p.oscMix, 0.0f, 1.0f);
    clampOne (p.filterCutoff, 0.05f, 0.95f);
    clampOne (p.filterRes, 0.0f, 1.0f);
    clampOne (p.filtEnvAmt, 0.0f, 1.0f);
    clampOne (p.keyTrack, 0.0f, 1.0f);
    clampOne (p.circuitDrive, 0.0f, 1.0f);
    clampOne (p.filterDrive, 0.0f, 1.0f);
    clampOne (p.unisonSpread, 0.0f, 40.0f);
    clampOne (p.glideMs, 0.0f, 500.0f);
    clampOne (p.ampAttack, 0.001f, 3.0f);
    clampOne (p.ampDecay, 0.001f, 3.0f);
    clampOne (p.ampSustain, 0.0f, 1.0f);
    clampOne (p.ampRelease, 0.001f, 5.0f);
    clampOne (p.filtAttack, 0.001f, 3.0f);
    clampOne (p.filtDecay, 0.001f, 3.0f);
    clampOne (p.filtSustain, 0.0f, 1.0f);
    clampOne (p.filtRelease, 0.001f, 5.0f);
    p.unisonVoices = static_cast<float> (std::clamp (static_cast<int> (p.unisonVoices + 0.5f), 1, 3));
}

bool paramsFinite (const SynthParams& p) noexcept
{
    auto ok = [] (float v) { return std::isfinite (v); };
    return ok (p.osc1Level) && ok (p.filterCutoff) && ok (p.ampAttack) && ok (p.glideMs);
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

std::string variationName (std::string_view foundationName, int variationNumber)
{
    std::ostringstream oss;
    oss << foundationName << " · V" << std::setw (3) << std::setfill ('0') << variationNumber;
    return oss.str();
}

std::vector<int> foundationIndicesForCategory (std::string_view category)
{
    std::vector<int> indices;
    const auto& foundations = getFoundationPresets();
    for (int i = 0; i < kFoundationPresetCount; ++i)
        if (foundations[static_cast<std::size_t> (i)].category == category)
            indices.push_back (i);
    return indices;
}

void appendCategory (std::vector<FactoryPreset>& library,
                     std::string_view category,
                     int targetCount)
{
    const auto& foundations = getFoundationPresets();
    const auto indices = foundationIndicesForCategory (category);
    if (indices.empty())
        throw std::runtime_error ("No foundations for category");

    int added = 0;
    for (int f : indices)
    {
        const auto& src = foundations[static_cast<std::size_t> (f)];
        FactoryPreset preset;
        preset.name = std::string (src.name);
        preset.category = std::string (category);
        preset.params = src.params;
        preset.foundationIndex = f;
        preset.isFoundation = true;
        library.push_back (std::move (preset));
        ++added;
    }

    int variationNumber = 1;
    std::size_t rr = 0;
    while (added < targetCount)
    {
        const int f = indices[rr % indices.size()];
        rr++;
        const auto& src = foundations[static_cast<std::size_t> (f)];

        FactoryPreset preset;
        preset.name = variationName (src.name, variationNumber);
        preset.category = std::string (category);
        preset.params = applyVariation (src.params, f, variationNumber);
        preset.foundationIndex = f;
        preset.isFoundation = false;
        library.push_back (std::move (preset));
        ++added;
        ++variationNumber;
    }
}

} // namespace

std::vector<FactoryPreset> PresetFactory::buildLibrary()
{
    std::vector<FactoryPreset> library;
    library.reserve (static_cast<std::size_t> (kFactoryPresetCount));

    for (const auto& q : kQuotas)
        appendCategory (library, q.category, q.count);

    if (static_cast<int> (library.size()) != kFactoryPresetCount)
        throw std::runtime_error ("Preset factory count mismatch");

    std::unordered_set<std::string> uniqueKeys;
    for (const auto& preset : library)
    {
        const std::string key = preset.category + "\0" + preset.name;
        if (! uniqueKeys.insert (key).second)
            throw std::runtime_error ("Duplicate preset name in category");
        if (! paramsFinite (preset.params))
            throw std::runtime_error ("Non-finite preset parameters");
    }

    return library;
}

} // namespace prophetrev2::presets
