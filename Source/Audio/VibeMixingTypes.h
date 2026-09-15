#pragma once

#include <JuceHeader.h>
#include "Internal/InternalPluginTypes.h"
#include <map>
#include <vector>

namespace vmpc::audio
{
/** One internal plug-in + parameter snapshot in the vibe chain. */
struct VibeSlotPreset
{
    int slotIndex = 0;
    internal::MixPluginId plugin = internal::MixPluginId::Gain;
    std::map<juce::String, float> parameters;
};

/** Result of interpreting a natural-language vibe brief. */
struct VibeMixRecipe
{
    juce::String title;
    juce::String explanation;
    std::vector<VibeSlotPreset> slots;
};

struct VibeMixInterpretResult
{
    bool success = false;
    juce::String error;
    VibeMixRecipe recipe;
};
} // namespace vmpc::audio
