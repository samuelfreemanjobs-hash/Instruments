#include "Assets/FactoryPatchLibrary.h"
#include "Assets/FactoryPatchDedup.h"

#include <iostream>
#include <set>
#include <unordered_map>

int main()
{
    const auto count = jdupgraded::assets::FactoryPatchLibrary::getPatchCount();
    if (count != jdupgraded::assets::kFactoryPatchCount || count != 500)
    {
        std::cerr << "JD Upgraded: expected 500 presets, got " << count << '\n';
        return 1;
    }

    std::set<std::string> names;
    std::unordered_map<std::uint64_t, std::size_t> fingerprints;
    int silentTones = 0;

    for (std::size_t i = 0; i < count; ++i)
    {
        const auto& patch = jdupgraded::assets::FactoryPatchLibrary::getPatch (i);
        const std::string name = patch.name;

        if (name.empty())
        {
            std::cerr << "Empty name at program " << i << '\n';
            return 1;
        }
        if (! names.insert (name).second)
        {
            std::cerr << "Duplicate name: " << name << " at program " << i << '\n';
            return 1;
        }

        const auto fp = jdupgraded::assets::detail::fingerprintPatch (patch);
        if (fingerprints.count (fp) > 0)
        {
            std::cerr << "Duplicate fingerprint: program " << i << " matches " << fingerprints[fp] << '\n';
            return 1;
        }
        fingerprints[fp] = i;

        float sum = 0.0f;
        for (const auto& tone : patch.tones)
            sum += tone.level;
        if (sum < 0.001f)
            ++silentTones;
    }

    std::cout << "JD Upgraded QA OK: " << count << " presets, " << silentTones << " silent (allowed).\n";
    return 0;
}
