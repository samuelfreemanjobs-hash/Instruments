#include "VibeMixEngine.h"
#include "VibeArchetypeLibrary.h"
#include "VibeTuning.h"

namespace resonance::audio
{
namespace
{
VibeTuning tuneFromBrief(const juce::String& trimmed, VibeMixTarget target)
{
    const auto brief = trimmed.toLowerCase().trim();
    VibeTuning tune;
    VibeArchetypeLibrary::applyBestArchetype(brief, tune);
    applyKeywordInfluences(brief, tune);
    applyTargetProfile(target.kind, tune);
    return tune;
}
} // namespace

VibeMixInterpretResult VibeMixEngine::interpretUserBrief(const juce::String& userText, VibeMixTarget target)
{
    VibeMixInterpretResult result;
    const auto trimmed = userText.trim();

    if (trimmed.isEmpty())
    {
        result.error = "Describe the vibe you want — e.g. \"warm punchy club master with airy top\".";
        return result;
    }

    const auto brief = trimmed.toLowerCase().trim();
    const auto conflicts = detectConflicts(brief);

    auto tune = tuneFromBrief(trimmed, target);
    result.recipe = buildRecipeFromTuning(trimmed, target, tune);
    result.recipe.conflicts.clear();
    for (const auto& c : conflicts)
        result.recipe.conflicts.push_back(c);

    if (result.recipe.slots.empty())
    {
        tune.useEq = true;
        tune.useComp = true;
        result.recipe = buildRecipeFromTuning(trimmed, target, tune);
    }

    auto brightTune = tune;
    brightTune.highDb += 3.0f;
    brightTune.midDb += 1.0f;
    brightTune.useTape = false;
    result.brighterVariant = buildRecipeFromTuning(trimmed + " (brighter)", target, brightTune);

    auto darkTune = tune;
    darkTune.highDb -= 3.0f;
    darkTune.lowDb += 2.0f;
    darkTune.useTape = true;
    darkTune.tapeDrive = juce::jmax(darkTune.tapeDrive, 0.3f);
    result.darkerVariant = buildRecipeFromTuning(trimmed + " (darker)", target, darkTune);

    result.success = !result.recipe.slots.empty();
    if (!result.success)
        result.error = "Could not map that brief to a mix chain.";

    return result;
}
} // namespace resonance::audio
