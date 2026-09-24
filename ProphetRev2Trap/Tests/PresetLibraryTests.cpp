#include "Presets/FactoryPresets.h"
#include "Presets/FeaturedPresets.h"
#include "Presets/PresetFactory.h"

#include <cmath>
#include <iostream>
#include <map>
#include <unordered_set>

namespace
{

bool inRange (float v, float lo, float hi) noexcept
{
    return std::isfinite (v) && v >= lo && v <= hi;
}

bool paramsInRange (const prophetrev2::SynthParams& p) noexcept
{
    if (! inRange (p.osc1Level, 0.0f, 1.0f)) return false;
    if (! inRange (p.osc2Level, 0.0f, 1.0f)) return false;
    if (! inRange (p.osc2DetuneCents, -50.0f, 50.0f)) return false;
    if (! inRange (p.oscMix, 0.0f, 1.0f)) return false;
    if (! inRange (p.filterCutoff, 0.05f, 0.95f)) return false;
    if (! inRange (p.filterRes, 0.0f, 1.0f)) return false;
    if (! inRange (p.filtEnvAmt, 0.0f, 1.0f)) return false;
    if (! inRange (p.keyTrack, 0.0f, 1.0f)) return false;
    if (! inRange (p.circuitDrive, 0.0f, 1.0f)) return false;
    if (! inRange (p.filterDrive, 0.0f, 1.0f)) return false;
    if (! inRange (p.unisonSpread, 0.0f, 40.0f)) return false;
    if (! inRange (p.glideMs, 0.0f, 500.0f)) return false;
    if (! inRange (p.ampAttack, 0.001f, 3.0f)) return false;
    if (! inRange (p.ampDecay, 0.001f, 3.0f)) return false;
    if (! inRange (p.ampSustain, 0.0f, 1.0f)) return false;
    if (! inRange (p.ampRelease, 0.001f, 5.0f)) return false;
    if (! inRange (p.filtAttack, 0.001f, 3.0f)) return false;
    if (! inRange (p.filtDecay, 0.001f, 3.0f)) return false;
    if (! inRange (p.filtSustain, 0.0f, 1.0f)) return false;
    if (! inRange (p.filtRelease, 0.001f, 5.0f)) return false;
    const int voices = static_cast<int> (p.unisonVoices + 0.5f);
    if (voices < 1 || voices > 3) return false;
    if (! std::isfinite (p.monoMode) || ! std::isfinite (p.legatoMode)) return false;
    return true;
}

} // namespace

int runPresetLibraryTests()
{
    const auto& lib = prophetrev2::presets::getFactoryPresets();
    if (static_cast<int> (lib.size()) != prophetrev2::presets::kFactoryPresetCount)
    {
        std::cerr << "Expected " << prophetrev2::presets::kFactoryPresetCount << " presets, got " << lib.size() << "\n";
        return 1;
    }

    int foundations = 0;
    std::map<std::string, int> byCategory;
    std::unordered_set<std::string> uniqueKeys;

    for (const auto& p : lib)
    {
        if (p.isFoundation)
            ++foundations;

        ++byCategory[p.category];

        const std::string key = p.category + "\0" + p.name;
        if (! uniqueKeys.insert (key).second)
        {
            std::cerr << "Duplicate category/name: " << p.category << " / " << p.name << "\n";
            return 1;
        }

        if (! paramsInRange (p.params))
        {
            std::cerr << "Parameter out of range: " << p.category << " / " << p.name << "\n";
            return 1;
        }
    }

    if (foundations != prophetrev2::presets::kFoundationPresetCount)
    {
        std::cerr << "Foundation count mismatch: expected " << prophetrev2::presets::kFoundationPresetCount
                  << ", got " << foundations << "\n";
        return 1;
    }

    static const std::map<std::string, int> kExpectedCounts = {
        { "Bass", 195 },
        { "Synth", 245 },
        { "Lead", 196 },
        { "Pad", 196 },
        { "Pluck/Keys", 196 },
    };

    for (const auto& [cat, expected] : kExpectedCounts)
    {
        const auto it = byCategory.find (cat);
        const int got = (it != byCategory.end()) ? it->second : 0;
        if (got != expected)
        {
            std::cerr << "Category count mismatch for " << cat << ": expected " << expected << ", got " << got << "\n";
            return 1;
        }
    }

    if (byCategory.size() != kExpectedCounts.size())
    {
        std::cerr << "Unexpected factory categories present\n";
        for (const auto& [cat, n] : byCategory)
            std::cerr << "  " << cat << ": " << n << "\n";
        return 1;
    }

    const int featured = prophetrev2::presets::countFeaturedInBank();
    if (featured < prophetrev2::presets::kFoundationPresetCount)
    {
        std::cerr << "Featured preset entries missing from bank (got " << featured << ")\n";
        return 1;
    }

    const auto rebuilt = prophetrev2::presets::PresetFactory::buildLibrary();
    if (lib[500].name != rebuilt[500].name)
    {
        std::cerr << "Preset factory is not deterministic\n";
        return 1;
    }

    return 0;
}
