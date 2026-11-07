#include "FactoryPatchDedup.h"

#include <cmath>

namespace jdupgraded::assets::detail
{

namespace
{

int quant (float v) noexcept
{
    return static_cast<int> (std::lround (v * 1000.0f));
}

} // namespace

std::uint64_t fingerprintPatch (const FactoryPatch& patch) noexcept
{
    std::uint64_t h = 1469598103934665603ull;
    auto mix = [&h] (std::uint64_t v)
    {
        h ^= v;
        h *= 1099511628211ull;
    };

    mix (static_cast<std::uint64_t> (patch.coupling));
    mix (static_cast<std::uint64_t> (quant (patch.masterGain)));
    mix (static_cast<std::uint64_t> (quant (patch.groupADrive)));
    mix (static_cast<std::uint64_t> (quant (patch.groupBMix)));

    for (const auto& tone : patch.tones)
    {
        mix (tone.multisampleSetId);
        mix (tone.waveIndex);
        mix (static_cast<std::uint64_t> (quant (tone.level)));
        mix (static_cast<std::uint64_t> (quant (tone.coarseSemis)));
        mix (static_cast<std::uint64_t> (quant (tone.filterCutoffNorm)));
        mix (static_cast<std::uint64_t> (quant (tone.filterResonanceNorm)));
    }
    return h;
}

void deduplicatePatchStore (std::vector<std::string>& names, std::vector<FactoryPatch>& patches)
{
    if (names.size() != patches.size())
        return;

    std::unordered_set<std::string> usedNames;
    std::unordered_map<std::uint64_t, std::size_t> usedFingerprints;

    for (std::size_t i = 0; i < patches.size(); ++i)
    {
        auto& patch = patches[i];
        auto& name = names[i];

        if (usedNames.count (name) > 0)
            name += " · P" + std::to_string (i + 1);
        usedNames.insert (name);

        std::uint64_t fp = fingerprintPatch (patch);
        int guard = 0;
        while (usedFingerprints.count (fp) > 0 && guard < 128)
        {
            patch.tones[0].waveIndex =
                static_cast<std::uint16_t> ((patch.tones[0].waveIndex + static_cast<std::uint16_t> (3 + guard)) % 128);
            patch.tones[2].waveIndex =
                static_cast<std::uint16_t> ((patch.tones[2].waveIndex + static_cast<std::uint16_t> (5 + guard)) % 128);
            fp = fingerprintPatch (patch);
            ++guard;
        }
        usedFingerprints[fp] = i;
        patch.name = name.c_str();
    }
}

} // namespace jdupgraded::assets::detail
