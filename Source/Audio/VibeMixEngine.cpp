#include "VibeMixEngine.h"

namespace vmpc::audio
{
namespace
{
struct VibeTuning
{
    float lowDb = 0.0f;
    float midDb = 0.0f;
    float highDb = 0.0f;
    float thresholdDb = -18.0f;
    float ratio = 3.0f;
    float attackMs = 10.0f;
    float releaseMs = 120.0f;
    float makeupDb = 0.0f;
    float ceilingDb = -0.3f;
    float limitReleaseMs = 50.0f;
    float gainDb = 0.0f;
    float pan = 0.0f;

    bool useEq = true;
    bool useComp = true;
    bool useLimiter = false;
    bool useGain = false;
    bool usePan = false;
};

juce::String normaliseBrief(juce::String text)
{
    return text.toLowerCase().trim();
}

bool containsAny(const juce::String& haystack, const juce::StringArray& needles)
{
    for (const auto& n : needles)
        if (haystack.contains(n))
            return true;
    return false;
}

void applyKeywordInfluences(const juce::String& brief, VibeTuning& tune)
{
    if (containsAny(brief, { "warm", "warmth", "cozy", "thick" }))
    {
        tune.lowDb += 3.0f;
        tune.highDb -= 1.0f;
        tune.releaseMs += 40.0f;
    }

    if (containsAny(brief, { "bright", "shiny", "sparkle", "crisp", "airy", "open" }))
    {
        tune.highDb += 4.0f;
        tune.midDb += 1.0f;
        tune.attackMs = juce::jmin(tune.attackMs, 8.0f);
    }

    if (containsAny(brief, { "dark", "muffled", "moody", "doom" }))
    {
        tune.highDb -= 4.0f;
        tune.lowDb += 2.0f;
    }

    if (containsAny(brief, { "punch", "punchy", "snappy", "drum", "drums", "transient" }))
    {
        tune.attackMs = 5.0f;
        tune.ratio = juce::jmax(tune.ratio, 4.5f);
        tune.thresholdDb = -16.0f;
        tune.makeupDb += 2.0f;
    }

    if (containsAny(brief, { "glue", "gluey", "bus", "cohesion" }))
    {
        tune.ratio = 2.5f;
        tune.releaseMs = 220.0f;
        tune.thresholdDb = -20.0f;
    }

    if (containsAny(brief, { "loud", "club", "master", "streaming", "limit", "final" }))
    {
        tune.useLimiter = true;
        tune.ceilingDb = -0.5f;
        tune.thresholdDb = -14.0f;
        tune.makeupDb += 3.0f;
        tune.useComp = true;
    }

    if (containsAny(brief, { "lofi", "lo-fi", "vintage", "tape", "cassette", "dusty" }))
    {
        tune.highDb -= 6.0f;
        tune.lowDb += 2.0f;
        tune.ratio = 4.0f;
        tune.thresholdDb = -22.0f;
        tune.releaseMs = 180.0f;
    }

    if (containsAny(brief, { "scooped", "scoops", "midrange", "hollow" }))
        tune.midDb -= 4.0f;

    if (containsAny(brief, { "vocal", "vocals", "voice", "singer" }))
    {
        tune.midDb += 3.0f;
        tune.ratio = 3.0f;
        tune.attackMs = 15.0f;
        tune.releaseMs = 140.0f;
    }

    if (containsAny(brief, { "gentle", "soft", "smooth", "transparent", "subtle" }))
    {
        tune.ratio = juce::jmin(tune.ratio, 2.0f);
        tune.thresholdDb = -26.0f;
        tune.attackMs = juce::jmax(tune.attackMs, 35.0f);
    }

    if (containsAny(brief, { "aggressive", "hard", "hype", "crush", "dirty" }))
    {
        tune.ratio = 8.0f;
        tune.thresholdDb = -12.0f;
        tune.useLimiter = true;
        tune.ceilingDb = -0.1f;
    }

    if (containsAny(brief, { "bass", "heavy", "sub", "808" }))
        tune.lowDb += 5.0f;

    if (containsAny(brief, { "wide", "stereo", "spacious" }))
    {
        tune.highDb += 2.0f;
        tune.usePan = true;
        tune.pan = 0.0f;
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

juce::String buildTitle(const juce::String& brief)
{
    if (brief.isEmpty())
        return "Custom vibe";

    const int maxLen = 48;
    if (brief.length() <= maxLen)
        return brief;

    return brief.substring(0, maxLen).trimEnd() + "…";
}

VibeMixRecipe buildRecipe(const juce::String& originalBrief, const VibeTuning& tune)
{
    VibeMixRecipe recipe;
    recipe.title = buildTitle(originalBrief.trim());
    recipe.explanation = "Vibe Mixing preset: ";

    juce::StringArray parts;
    if (tune.useEq)
        parts.add("EQ-3");
    if (tune.useComp)
        parts.add("Compressor");
    if (tune.useLimiter)
        parts.add("Limiter");
    if (tune.useGain)
        parts.add("Gain");
    if (tune.usePan)
        parts.add("Pan");

    recipe.explanation += parts.joinIntoString(" → ");

    int nextSlot = 0;

    if (tune.useEq)
    {
        VibeSlotPreset slot;
        slot.slotIndex = nextSlot++;
        slot.plugin = internal::MixPluginId::Eq3;
        slot.parameters = { { "low", tune.lowDb }, { "mid", tune.midDb }, { "high", tune.highDb } };
        recipe.slots.push_back(slot);
    }

    if (tune.useComp)
    {
        VibeSlotPreset slot;
        slot.slotIndex = nextSlot++;
        slot.plugin = internal::MixPluginId::Compressor;
        slot.parameters = { { "threshold", tune.thresholdDb },
                              { "ratio", tune.ratio },
                              { "attack", tune.attackMs },
                              { "release", tune.releaseMs },
                              { "makeup", tune.makeupDb } };
        recipe.slots.push_back(slot);
    }

    if (tune.useLimiter)
    {
        VibeSlotPreset slot;
        slot.slotIndex = nextSlot++;
        slot.plugin = internal::MixPluginId::Limiter;
        slot.parameters = { { "ceiling", tune.ceilingDb }, { "release", tune.limitReleaseMs } };
        recipe.slots.push_back(slot);
    }

    if (tune.useGain)
    {
        VibeSlotPreset slot;
        slot.slotIndex = nextSlot++;
        slot.plugin = internal::MixPluginId::Gain;
        slot.parameters = { { "gainDb", tune.gainDb } };
        recipe.slots.push_back(slot);
    }

    if (tune.usePan)
    {
        VibeSlotPreset slot;
        slot.slotIndex = nextSlot++;
        slot.plugin = internal::MixPluginId::Pan;
        slot.parameters = { { "pan", tune.pan } };
        recipe.slots.push_back(slot);
    }

    return recipe;
}
} // namespace

VibeMixInterpretResult VibeMixEngine::interpretUserBrief(const juce::String& userText)
{
    VibeMixInterpretResult result;
    const auto trimmed = userText.trim();

    if (trimmed.isEmpty())
    {
        result.error = "Describe the vibe you want — e.g. \"warm punchy club master with airy top\".";
        return result;
    }

    const auto brief = normaliseBrief(trimmed);
    VibeTuning tune;
    applyKeywordInfluences(brief, tune);

    result.recipe = buildRecipe(trimmed, tune);

    if (result.recipe.slots.empty())
    {
        tune.useEq = true;
        tune.useComp = true;
        result.recipe = buildRecipe(trimmed, tune);
    }

    result.success = !result.recipe.slots.empty();
    if (!result.success)
        result.error = "Could not map that brief to a mix chain.";

    return result;
}
} // namespace vmpc::audio
