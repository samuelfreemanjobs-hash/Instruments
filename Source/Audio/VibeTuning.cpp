#include "VibeTuning.h"
#include "PluginSlotChain.h"
#include "Internal/InternalPluginTypes.h"

namespace resonance::audio
{
namespace
{
bool containsAny(const juce::String& haystack, const juce::StringArray& needles)
{
    for (const auto& n : needles)
        if (haystack.contains(n))
            return true;
    return false;
}

juce::String buildTitle(const juce::String& brief)
{
    if (brief.isEmpty())
        return "Custom vibe";
    const int maxLen = 48;
    if (brief.length() <= maxLen)
        return brief;
    return brief.substring(0, maxLen).trimEnd() + "…";
}
} // namespace

void applyKeywordInfluences(const juce::String& brief, VibeTuning& tune)
{
    if (containsAny(brief, { "warm", "warmth", "cozy", "thick" }))
    {
        tune.lowDb += 3.0f;
        tune.highDb -= 1.0f;
        tune.releaseMs += 40.0f;
        tune.rationales.add("'warm' → fuller lows, softer top");
    }

    if (containsAny(brief, { "bright", "shiny", "sparkle", "crisp", "airy", "open" }))
    {
        tune.highDb += 4.0f;
        tune.midDb += 1.0f;
        tune.attackMs = juce::jmin(tune.attackMs, 8.0f);
        tune.rationales.add("'bright/airy' → lifted highs, faster attack");
    }

    if (containsAny(brief, { "dark", "muffled", "moody", "doom" }))
    {
        tune.highDb -= 4.0f;
        tune.lowDb += 2.0f;
        tune.rationales.add("'dark' → rolled highs, weight in lows");
    }

    if (containsAny(brief, { "punch", "punchy", "snappy", "drum", "drums", "transient" }))
    {
        tune.attackMs = 5.0f;
        tune.ratio = juce::jmax(tune.ratio, 4.5f);
        tune.thresholdDb = -16.0f;
        tune.makeupDb += 2.0f;
        tune.rationales.add("'punchy' → fast comp, more ratio");
    }

    if (containsAny(brief, { "glue", "gluey", "bus", "cohesion" }))
    {
        tune.ratio = 2.5f;
        tune.releaseMs = 220.0f;
        tune.thresholdDb = -20.0f;
        tune.rationales.add("'glue' → slow release bus compression");
    }

    if (containsAny(brief, { "loud", "club", "master", "streaming", "limit", "final" }))
    {
        tune.useLimiter = true;
        tune.ceilingDb = -0.5f;
        tune.thresholdDb = -14.0f;
        tune.makeupDb += 3.0f;
        tune.useComp = true;
        tune.rationales.add("'loud/master' → limiter on, hotter makeup");
    }

    if (containsAny(brief, { "lofi", "lo-fi", "vintage", "tape", "cassette", "dusty" }))
    {
        tune.highDb -= 6.0f;
        tune.lowDb += 2.0f;
        tune.ratio = 4.0f;
        tune.thresholdDb = -22.0f;
        tune.releaseMs = 180.0f;
        tune.useTape = true;
        tune.tapeDrive = 0.35f;
        tune.rationales.add("'lofi/tape' → darker EQ + tape saturation");
    }

    if (containsAny(brief, { "scooped", "scoops", "midrange", "hollow" }))
    {
        tune.midDb -= 4.0f;
        tune.rationales.add("'scooped' → mid dip");
    }

    if (containsAny(brief, { "vocal", "vocals", "voice", "singer" }))
    {
        tune.midDb += 3.0f;
        tune.ratio = 3.0f;
        tune.attackMs = 15.0f;
        tune.releaseMs = 140.0f;
        tune.useDeEsser = true;
        tune.rationales.add("'vocals' → mid focus + de-esser");
    }

    if (containsAny(brief, { "gentle", "soft", "smooth", "transparent", "subtle" }))
    {
        tune.ratio = juce::jmin(tune.ratio, 2.0f);
        tune.thresholdDb = -26.0f;
        tune.attackMs = juce::jmax(tune.attackMs, 35.0f);
        tune.rationales.add("'gentle' → light compression");
    }

    if (containsAny(brief, { "aggressive", "hard", "hype", "crush", "dirty" }))
    {
        tune.ratio = 8.0f;
        tune.thresholdDb = -12.0f;
        tune.useLimiter = true;
        tune.ceilingDb = -0.1f;
        tune.useTape = true;
        tune.tapeDrive = 0.5f;
        tune.rationales.add("'aggressive' → heavy comp + saturation");
    }

    if (containsAny(brief, { "bass", "heavy", "sub", "808" }))
    {
        tune.lowDb += 5.0f;
        tune.rationales.add("'bass' → low shelf boost");
    }

    if (containsAny(brief, { "wide", "stereo", "spacious" }))
    {
        tune.useWidth = true;
        tune.width = 1.35f;
        tune.rationales.add("'wide' → stereo width");
    }

    if (containsAny(brief, { "clean", "mud", "rumble", "tight" }))
    {
        tune.useHpf = true;
        tune.hpfHz = 90.0f;
        tune.rationales.add("'clean/tight' → HPF");
    }

    if (containsAny(brief, { "sibilance", "harsh", "ess", "de-ess", "deess" }))
    {
        tune.useDeEsser = true;
        tune.rationales.add("'sibilance' → de-esser");
    }

    if (containsAny(brief, { "quiet", "bedroom", "low volume" }))
    {
        tune.gainDb = -3.0f;
        tune.useGain = true;
    }

    if (containsAny(brief, { "boost", "louder", "hot" }))
    {
        tune.gainDb += 2.5f;
        tune.useGain = true;
    }

    if (containsAny(brief, { "only eq", "just eq", "tone only" }))
    {
        tune.useComp = false;
        tune.useLimiter = false;
    }

    if (containsAny(brief, { "no limiter", "without limiter" }))
        tune.useLimiter = false;
}

void applyTargetProfile(VibeMixTargetKind target, VibeTuning& tune)
{
    switch (target)
    {
        case VibeMixTargetKind::Drums:
            tune.useHpf = true;
            tune.hpfHz = juce::jmax(tune.hpfHz, 60.0f);
            tune.attackMs = juce::jmin(tune.attackMs, 8.0f);
            tune.rationales.add("Drums profile → HPF + punchy comp");
            break;
        case VibeMixTargetKind::Vocals:
            tune.useDeEsser = true;
            tune.midDb += 2.0f;
            tune.rationales.add("Vocals profile → de-esser + mid");
            break;
        case VibeMixTargetKind::Synths:
            tune.useWidth = true;
            tune.width = juce::jmax(tune.width, 1.2f);
            tune.highDb += 1.5f;
            tune.rationales.add("Synths profile → width + shine");
            break;
        case VibeMixTargetKind::FullMix:
            tune.useComp = true;
            tune.rationales.add("Full mix profile → balanced bus");
            break;
        case VibeMixTargetKind::Master:
            tune.rationales.add("Master bus target");
            break;
        case VibeMixTargetKind::Channel:
            tune.rationales.add("Channel insert profile → compact 2-slot chain");
            break;
    }
}

juce::StringArray detectConflicts(const juce::String& brief)
{
    juce::StringArray conflicts;
    const bool bright = containsAny(brief, { "bright", "airy", "sparkle", "crisp" });
    const bool dark = containsAny(brief, { "dark", "lofi", "muffled", "dusty" });
    if (bright && dark)
        conflicts.add("You asked for both bright and dark/lofi — the main preset balances toward your dominant words; try Brighter or Darker variants.");

    const bool loud = containsAny(brief, { "loud", "club", "hot" });
    const bool quiet = containsAny(brief, { "quiet", "gentle", "bedroom" });
    if (loud && quiet)
        conflicts.add("Both 'loud' and 'quiet' detected — gain staging may need manual tweak.");

    return conflicts;
}

VibeMixRecipe buildRecipeFromTuning(const juce::String& title, const VibeMixTarget& target, const VibeTuning& tune)
{
    VibeMixRecipe recipe;
    recipe.title = buildTitle(title);
    recipe.target = target;
    recipe.explanation = "Vibe Mixing → " + target.displayName() + ": ";

    juce::StringArray parts;
    int nextSlot = 0;
    const int maxSlots = target.kind == VibeMixTargetKind::Channel ? PluginSlotChain::kChannelInsertSlots
                                                                   : PluginSlotChain::kMasterSlots;

    auto addSlot = [&](internal::MixPluginId id, const std::map<juce::String, float>& params, const juce::String& why) {
        if (nextSlot >= maxSlots)
            return;
        VibeSlotPreset slot;
        slot.slotIndex = nextSlot++;
        slot.plugin = id;
        slot.parameters = params;
        slot.rationale = why;
        recipe.slots.push_back(slot);
        for (const auto& info : internal::allMixPlugins())
        {
            if (info.id == id)
            {
                parts.add(info.displayName);
                break;
            }
        }
    };

    if (tune.useHpf)
        addSlot(internal::MixPluginId::Hpf, { { "cutoff", tune.hpfHz } }, "High-pass cleanup");

    if (tune.useEq)
        addSlot(internal::MixPluginId::Eq3,
                { { "low", tune.lowDb }, { "mid", tune.midDb }, { "high", tune.highDb } },
                "Tone shaping");

    if (tune.useDeEsser)
        addSlot(internal::MixPluginId::DeEsser, { { "threshold", -18.0f }, { "amount", 0.55f } }, "Sibilance control");

    if (tune.useComp)
        addSlot(internal::MixPluginId::Compressor,
                { { "threshold", tune.thresholdDb },
                  { "ratio", tune.ratio },
                  { "attack", tune.attackMs },
                  { "release", tune.releaseMs },
                  { "makeup", tune.makeupDb } },
                "Dynamics");

    if (tune.useTape)
        addSlot(internal::MixPluginId::TapeSat, { { "drive", tune.tapeDrive } }, "Harmonic saturation");

    if (tune.useWidth)
        addSlot(internal::MixPluginId::StereoWidth, { { "width", tune.width } }, "Stereo image");

    if (tune.useLimiter)
        addSlot(internal::MixPluginId::Limiter,
                { { "ceiling", tune.ceilingDb }, { "release", tune.limitReleaseMs } },
                "Peak control");

    if (tune.useGain)
        addSlot(internal::MixPluginId::Gain, { { "gainDb", tune.gainDb } }, "Level trim");

    if (tune.usePan)
        addSlot(internal::MixPluginId::Pan, { { "pan", tune.pan } }, "Pan");

    recipe.explanation += parts.joinIntoString(" → ");
    return recipe;
}
} // namespace resonance::audio
