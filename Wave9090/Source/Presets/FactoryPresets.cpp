#include "Presets/FactoryPresets.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace wave9090::presets
{

namespace
{

enum class PresetCategory : std::uint8_t
{
    trap = 0,
    phonk,
    drill,
    bell,
    pad,
    lead,
    pluck,
    keys,
    synth,
    mpc
};

constexpr std::size_t kCategories = 10;
constexpr std::size_t kPerCategory = kFactoryPresetCount / kCategories;

PresetCategory categoryForIndex (std::size_t index) noexcept
{
    return static_cast<PresetCategory> (std::min (index / kPerCategory, kCategories - 1));
}

const char* categoryLabel (PresetCategory c) noexcept
{
    switch (c)
    {
        case PresetCategory::trap: return "TRAP";
        case PresetCategory::phonk: return "PHONK";
        case PresetCategory::drill: return "DRILL";
        case PresetCategory::bell: return "BELL";
        case PresetCategory::pad: return "PAD";
        case PresetCategory::lead: return "LEAD";
        case PresetCategory::pluck: return "PLUCK";
        case PresetCategory::keys: return "KEYS";
        case PresetCategory::synth: return "SYNTH";
        case PresetCategory::mpc: return "MPC";
        default: return "TRAP";
    }
}

float frand (std::size_t index, int salt, float lo, float hi) noexcept
{
    const float t = static_cast<float> ((index * 1103515245u + static_cast<std::uint32_t> (salt) * 7919u) % 10000u) / 10000.0f;
    return lo + (hi - lo) * t;
}

SynthParams baseParams() noexcept
{
    return SynthParams {};
}

SynthParams buildParams (std::size_t index) noexcept
{
    const auto category = categoryForIndex (index);
    const std::size_t slot = index % kPerCategory;
    const float v = static_cast<float> (slot);

    SynthParams p = baseParams();

    switch (category)
    {
        case PresetCategory::trap:
            p.wtPosition = frand (index, 1, 0.05f, 0.22f);
            p.filterCutoff = frand (index, 2, 0.28f, 0.52f);
            p.filterRes = frand (index, 3, 0.1f, 0.35f);
            p.subLevel = frand (index, 4, 0.75f, 1.0f);
            p.drive = frand (index, 5, 0.35f, 0.75f);
            p.crush = frand (index, 6, 0.15f, 0.45f);
            p.filterDestructive = (slot % 3) != 2;
            p.ampAttack = 0.001f;
            p.ampDecay = frand (index, 7, 0.25f, 0.55f);
            p.ampSustain = 0.9f;
            p.ampRelease = frand (index, 8, 0.2f, 0.45f);
            break;

        case PresetCategory::phonk:
            p.wtPosition = frand (index, 9, 0.4f, 0.7f);
            p.filterCutoff = frand (index, 10, 0.55f, 0.82f);
            p.drive = frand (index, 11, 0.5f, 0.9f);
            p.crush = frand (index, 12, 0.25f, 0.65f);
            p.detuneCents = frand (index, 13, -22.0f, -6.0f);
            p.filterDestructive = true;
            p.tapeWobble = frand (index, 14, 0.1f, 0.45f);
            break;

        case PresetCategory::drill:
            p.wtPosition = frand (index, 15, 0.65f, 0.95f);
            p.filterCutoff = frand (index, 16, 0.4f, 0.7f);
            p.filterRes = frand (index, 17, 0.35f, 0.7f);
            p.drive = frand (index, 18, 0.45f, 0.85f);
            p.filterDestructive = true;
            p.ampAttack = 0.002f;
            p.ampDecay = frand (index, 19, 0.08f, 0.25f);
            p.ampSustain = frand (index, 20, 0.0f, 0.2f);
            break;

        case PresetCategory::bell:
            p.wtPosition = frand (index, 21, 0.55f, 0.92f);
            p.filterCutoff = frand (index, 22, 0.68f, 0.9f);
            p.filterRes = frand (index, 23, 0.25f, 0.55f);
            p.ampDecay = frand (index, 24, 0.35f, 0.85f);
            p.ampRelease = frand (index, 25, 0.5f, 1.2f);
            break;

        case PresetCategory::pad:
            p.wtPosition = frand (index, 26, 0.18f, 0.5f);
            p.wtScan = frand (index, 27, 0.05f, 0.4f);
            p.tapeWobble = frand (index, 28, 0.2f, 0.75f);
            p.ampAttack = frand (index, 29, 0.2f, 0.75f);
            p.ampRelease = frand (index, 30, 0.6f, 1.5f);
            p.stereoWidth = frand (index, 31, 0.2f, 0.75f);
            break;

        case PresetCategory::lead:
            p.wtPosition = frand (index, 32, 0.45f, 0.88f);
            p.wtScan = frand (index, 33, 0.1f, 0.55f);
            p.filterCutoff = frand (index, 34, 0.52f, 0.85f);
            p.detuneCents = frand (index, 35, -12.0f, 12.0f);
            p.glideMs = (slot % 4 == 0) ? frand (index, 36, 40.0f, 180.0f) : 0.0f;
            break;

        case PresetCategory::pluck:
            p.wtPosition = frand (index, 37, 0.35f, 0.75f);
            p.filterCutoff = frand (index, 38, 0.52f, 0.88f);
            p.filtEnvAmt = frand (index, 39, 0.45f, 0.9f);
            p.ampAttack = 0.001f;
            p.ampDecay = frand (index, 40, 0.06f, 0.22f);
            p.ampSustain = 0.0f;
            p.ampRelease = frand (index, 41, 0.15f, 0.35f);
            break;

        case PresetCategory::keys:
            p.wtPosition = frand (index, 42, 0.25f, 0.65f);
            p.tapeWobble = frand (index, 43, 0.15f, 0.55f);
            p.stereoWidth = frand (index, 44, 0.15f, 0.45f);
            p.wtScan = frand (index, 45, 0.0f, 0.25f);
            p.filterCutoff = frand (index, 46, 0.55f, 0.8f);
            break;

        case PresetCategory::synth:
            p.wtPosition = frand (index, 47, 0.3f, 0.95f);
            p.wtScan = frand (index, 48, 0.1f, 0.6f);
            p.filterCutoff = frand (index, 49, 0.4f, 0.9f);
            p.filterRes = frand (index, 50, 0.2f, 0.75f);
            p.stereoWidth = frand (index, 51, 0.2f, 0.8f);
            break;

        case PresetCategory::mpc:
        default:
            p.wtPosition = frand (index, 52, 0.04f, 0.2f);
            p.subLevel = frand (index, 53, 0.8f, 1.0f);
            p.drive = frand (index, 54, 0.35f, 0.8f);
            p.crush = frand (index, 55, 0.15f, 0.55f);
            p.filterCutoff = frand (index, 56, 0.25f, 0.5f);
            p.filterDestructive = (slot % 2) == 0;
            break;
    }

    (void) v;
    return p;
}

void buildMarketName (std::size_t index, PresetCategory category, std::string& out)
{
    static constexpr const char* kTrapAdj[8] = { "Slide", "Night", "Buried", "Ghost", "Rack", "Low", "Dirty", "Zone" };
    static constexpr const char* kTrapNoun[8] = { "808", "Sub", "Kick", "808+", "Bass", "Pulse", "Thump", "Submax" };
    static constexpr const char* kPhonkAdj[8] = { "Memphis", "Dusty", "Grim", "Chopped", "Vintage", "Swamp", "Ritual", "Grave" };
    static constexpr const char* kPhonkNoun[8] = { "Cowbell", "Bell", "Lead", "Stab", "Keys", "Flute", "Organ", "Horn" };
    static constexpr const char* kDrillAdj[8] = { "Drill", "Ice", "Shard", "Slap", "Metro", "Chrome", "Cold", "Steel" };
    static constexpr const char* kDrillNoun[8] = { "Stab", "Pluck", "Lead", "Bell", "Siren", "Chop", "Hit", "Strike" };
    static constexpr const char* kGenericAdj[8] = { "Dark", "Neon", "Haze", "Velvet", "Static", "Glitch", "Satin", "Ritual" };
    static constexpr const char* kGenericNoun[8] = { "Keys", "Pad", "Bell", "Lead", "Pluck", "Air", "Tone", "Wash" };

    const std::size_t slot = index % kPerCategory;
    const std::size_t a = slot % 8;
    const std::size_t n = (slot / 3) % 8;

    char buf[64];
    switch (category)
    {
        case PresetCategory::trap:
            std::snprintf (buf, sizeof (buf), "%s %s %02zu · P%03zu", kTrapAdj[a], kTrapNoun[n], slot + 1, index + 1);
            break;
        case PresetCategory::phonk:
            std::snprintf (buf, sizeof (buf), "%s %s %02zu · P%03zu", kPhonkAdj[a], kPhonkNoun[n], slot + 1, index + 1);
            break;
        case PresetCategory::drill:
            std::snprintf (buf, sizeof (buf), "%s %s %02zu · P%03zu", kDrillAdj[a], kDrillNoun[n], slot + 1, index + 1);
            break;
        default:
            std::snprintf (buf,
                           sizeof (buf),
                           "%s %s %02zu · P%03zu",
                           kGenericAdj[a],
                           kGenericNoun[n],
                           slot + 1,
                           index + 1);
            break;
    }
    out = buf;
}

std::uint64_t fingerprintParams (const SynthParams& p) noexcept
{
    auto q = [] (float v) { return static_cast<std::uint32_t> (std::lround (v * 1000.0f)); };
    std::uint64_t h = 1469598103934665603ull;
    auto mix = [&h] (std::uint64_t v)
    {
        h ^= v;
        h *= 1099511628211ull;
    };
    mix (q (p.wtPosition));
    mix (q (p.wtScan));
    mix (q (p.filterCutoff));
    mix (q (p.filterRes));
    mix (p.filterDestructive ? 1u : 0u);
    mix (q (p.drive));
    mix (q (p.crush));
    mix (q (p.subLevel));
    mix (q (p.detuneCents));
    mix (q (p.tapeWobble));
    mix (q (p.stereoWidth));
    return h;
}

struct PresetStore final
{
    std::vector<std::string> names;
    std::vector<std::string> categories;
    std::vector<FactoryPreset> presets;
};

void deduplicatePresetStore (PresetStore& store)
{
    std::unordered_set<std::string> usedNames;
    std::unordered_map<std::uint64_t, std::size_t> usedFingerprints;

    for (std::size_t i = 0; i < store.presets.size(); ++i)
    {
        auto& preset = store.presets[i];
        auto& name = store.names[i];

        if (usedNames.count (name) > 0)
            name += " · alt";
        usedNames.insert (name);

        std::uint64_t fp = fingerprintParams (preset.params);
        int guard = 0;
        while (usedFingerprints.count (fp) > 0 && guard < 64)
        {
            preset.params.wtPosition = std::fmod (preset.params.wtPosition + 0.013f + guard * 0.002f, 1.0f);
            preset.params.drive = std::min (1.0f, preset.params.drive + 0.01f);
            fp = fingerprintParams (preset.params);
            ++guard;
        }
        usedFingerprints[fp] = i;

        preset.name = name;
        preset.category = store.categories[i];
    }
}

void applyCuratedOverrides (std::size_t index, SynthParams& p, std::string& name, std::string_view& category)
{
    switch (index)
    {
        case 0:
            name = "Slide 808";
            category = "TRAP";
            p = buildParams (index);
            p.wtPosition = 0.08f;
            p.filterCutoff = 0.35f;
            p.subLevel = 0.95f;
            p.drive = 0.45f;
            p.crush = 0.25f;
            p.filterDestructive = true;
            break;
        case 1:
            name = "Night Drive Lead";
            category = "TRAP";
            p.wtPosition = 0.62f;
            p.wtScan = 0.35f;
            p.tapeWobble = 0.4f;
            break;
        case 2:
            name = "Dark Trap Pluck";
            category = "TRAP";
            p.wtPosition = 0.48f;
            p.ampDecay = 0.12f;
            p.ampSustain = 0.0f;
            break;
        case 3:
            name = "Funeral Bell";
            category = "TRAP";
            p.wtPosition = 0.78f;
            break;
        case 55:
            name = "Memphis Cowbell";
            category = "PHONK";
            p.wtPosition = 0.55f;
            p.drive = 0.65f;
            p.detuneCents = -14.0f;
            break;
        case 105:
            name = "Drill Stab";
            category = "DRILL";
            p.wtPosition = 0.88f;
            p.filterDestructive = true;
            break;
        case 400:
            name = "Trap Pad";
            category = "PAD";
            p.tapeWobble = 0.65f;
            break;
        case 450:
            name = "MPC Submax";
            category = "MPC";
            p.subLevel = 0.98f;
            break;
        default:
            break;
    }
}

const PresetStore& presetStore()
{
    static PresetStore store = []() -> PresetStore
    {
        PresetStore result;
        result.names.resize (kFactoryPresetCount);
        result.categories.resize (kFactoryPresetCount);
        result.presets.resize (kFactoryPresetCount);

        for (std::size_t i = 0; i < kFactoryPresetCount; ++i)
        {
            const auto category = categoryForIndex (i);
            auto params = buildParams (i);
            buildMarketName (i, category, result.names[i]);
            result.categories[i] = categoryLabel (category);

            std::string_view catView = result.categories[i];
            applyCuratedOverrides (i, params, result.names[i], catView);
            result.categories[i] = std::string (catView);

            result.presets[i].name = result.names[i];
            result.presets[i].category = result.categories[i];
            result.presets[i].params = params;
        }
        deduplicatePresetStore (result);
        return result;
    }();
    return store;
}

} // namespace

const std::vector<FactoryPreset>& getFactoryPresets() noexcept
{
    return presetStore().presets;
}

std::size_t getFactoryPresetCount() noexcept
{
    return kFactoryPresetCount;
}

const char* getFactoryPresetCategory (std::size_t index) noexcept
{
    const auto& store = presetStore();
    if (index >= store.categories.size())
        return "TRAP";
    return store.categories[index].c_str();
}

} // namespace wave9090::presets
