#pragma once

#include "VibeMixingTypes.h"

namespace resonance::audio
{
/** JSON export/import for vibe recipes (schema v1). */
class VibeRecipeJson
{
public:
    static juce::String toJson(const VibeMixRecipe& recipe);
    static bool fromJson(const juce::String& json, VibeMixRecipe& out, juce::String& error);
};
} // namespace resonance::audio
