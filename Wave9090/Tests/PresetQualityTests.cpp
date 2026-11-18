#include "Presets/FactoryPresets.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <set>
#include <unordered_map>

namespace
{

std::uint64_t fingerprint (const wave9090::SynthParams& p)
{
    auto q = [] (float v) { return static_cast<std::uint32_t> (std::lround (v * 1000.0f)); };
    std::uint64_t h = 1469598103934665603ull;
    auto mix = [&h] (std::uint64_t v)
    {
        h ^= v;
        h *= 1099511628211ull;
    };
    mix (q (p.wtPosition));
    mix (q (p.filterCutoff));
    mix (q (p.drive));
    mix (q (p.subLevel));
    mix (p.filterDestructive ? 1u : 0u);
    return h;
}

} // namespace

int main()
{
    const auto& presets = wave9090::presets::getFactoryPresets();
    if (presets.size() != 500)
    {
        std::cerr << "Expected 500 presets, got " << presets.size() << '\n';
        return EXIT_FAILURE;
    }

    std::set<std::string> names;
    std::unordered_map<std::uint64_t, std::size_t> fingerprints;

    for (std::size_t i = 0; i < presets.size(); ++i)
    {
        const auto& preset = presets[i];
        const std::string name (preset.name);
        if (name.empty())
        {
            std::cerr << "Empty name at " << i << '\n';
            return EXIT_FAILURE;
        }
        if (! names.insert (name).second)
        {
            std::cerr << "Duplicate name: " << name << " at " << i << '\n';
            return EXIT_FAILURE;
        }

        const auto fp = fingerprint (preset.params);
        if (fingerprints.count (fp) > 0)
        {
            std::cerr << "Duplicate sound at " << i << " vs " << fingerprints[fp] << '\n';
            return EXIT_FAILURE;
        }
        fingerprints[fp] = i;
    }

    std::cout << "WAVE-9090 QA OK: 500 unique names and fingerprints\n";
    return EXIT_SUCCESS;
}
