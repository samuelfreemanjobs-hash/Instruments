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

// Sound-design subagent expands this list; featured seeds all authored foundations.
constexpr FeaturedKey kFeatured[] = {
    { "Bass", "Traproom Sub" },
    { "Bass", "Gucci Glide Bass" },
    { "Bass", "Shawty Dark Bass" },
    { "Bass", "Low Circuit" },
    { "Bass", "Wavephonk Knock" },
    { "Bass", "Cloud Phonk Mist" },
    { "Bass", "Wavephonk Slump" },
    { "Bass", "Vapor Trap Sub" },
    { "Bass", "Cloud Phonk Low" },
    { "Synth", "Night Circuit" },
    { "Synth", "Vaporwave Cruise" },
    { "Synth", "Jeezy Stack Choir" },
    { "Synth", "Neon Hall Chords" },
    { "Synth", "Redd Key Stab" },
    { "Synth", "Bourne Glass Keys" },
    { "Synth", "Wave Horizon" },
    { "Synth", "Vapor Trap Glass" },
    { "Synth", "Hardware Juno Stack" },
    { "Synth", "Wave Midnight" },
    { "Synth", "Chillwave VHS" },
    { "Synth", "Ambient Phonk Layer" },
    { "Synth", "Hardware Poly Drive" },
    { "Synth", "Vapor Trap Stack" },
    { "Lead", "Pierre Dream Lead" },
    { "Lead", "Jeezy Square Lead" },
    { "Lead", "London Bright Lead" },
    { "Lead", "Wheezy Siren" },
    { "Lead", "Wavephonk Bell" },
    { "Lead", "Wavephonk Memphis" },
    { "Lead", "Vapor Trap Hook" },
    { "Lead", "Hardware Solo Lead" },
    { "Lead", "Cloud Phonk Air" },
    { "Pad", "Mike Dean Haze" },
    { "Pad", "Dean Analog Pad" },
    { "Pad", "Void Circuit Pad" },
    { "Pad", "Chillwave Drift" },
    { "Pad", "Ambient Phonk Void" },
    { "Pad", "Chillwave Sunset" },
    { "Pad", "Cloud Phonk Haze" },
    { "Pad", "Ambient Phonk Smoke" },
    { "Pluck/Keys", "Mike Will Pluck" },
    { "Pluck/Keys", "Wheezy Short Pluck" },
    { "Pluck/Keys", "Trap Wire Pluck" },
    { "Pluck/Keys", "Staccato Click" },
    { "Pluck/Keys", "Vapor Trap Click" },
    { "Pluck/Keys", "Wave Tape Keys" },
    { "Pluck/Keys", "Cloud Phonk Bell" },
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
