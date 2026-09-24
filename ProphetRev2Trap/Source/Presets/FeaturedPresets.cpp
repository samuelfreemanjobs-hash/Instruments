#include "Presets/FeaturedPresets.h"

#include "Presets/FactoryPresets.h"

#include <unordered_set>

namespace prophetrev2::presets
{

namespace
{

struct FeaturedKey
{
    std::string_view category;
    std::string_view name;
};

// Sound-design subagent expands this list; v0.1 seeds all 21 authored foundations.
constexpr FeaturedKey kFeatured[] = {
    { "Bass", "Traproom Sub" },
    { "Bass", "Gucci Glide Bass" },
    { "Bass", "Shawty Dark Bass" },
    { "Bass", "Low Circuit" },
    { "Synth", "Night Circuit" },
    { "Synth", "Vaporwave Cruise" },
    { "Synth", "Jeezy Stack Choir" },
    { "Synth", "Neon Hall Chords" },
    { "Synth", "Redd Key Stab" },
    { "Synth", "Bourne Glass Keys" },
    { "Lead", "Pierre Dream Lead" },
    { "Lead", "Jeezy Square Lead" },
    { "Lead", "London Bright Lead" },
    { "Lead", "Wheezy Siren" },
    { "Pad", "Mike Dean Haze" },
    { "Pad", "Dean Analog Pad" },
    { "Pad", "Void Circuit Pad" },
    { "Pluck/Keys", "Mike Will Pluck" },
    { "Pluck/Keys", "Wheezy Short Pluck" },
    { "Pluck/Keys", "Trap Wire Pluck" },
    { "Pluck/Keys", "Staccato Click" },
};

std::string makeKey (std::string_view category, std::string_view name)
{
    return std::string (category) + '\0' + std::string (name);
}

const std::unordered_set<std::string>& featuredKeySet()
{
    static const std::unordered_set<std::string> keys = [] {
        std::unordered_set<std::string> s;
        s.reserve (std::size (kFeatured));
        for (const auto& f : kFeatured)
            s.insert (makeKey (f.category, f.name));
        return s;
    }();
    return keys;
}

} // namespace

bool isFeaturedFactoryPreset (std::string_view category, std::string_view name) noexcept
{
    return featuredKeySet().contains (makeKey (category, name));
}

int countFeaturedInBank() noexcept
{
    int n = 0;
    for (const auto& p : getFactoryPresets())
        if (isFeaturedFactoryPreset (p.category, p.name))
            ++n;
    return n;
}

} // namespace prophetrev2::presets
