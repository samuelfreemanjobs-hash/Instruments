#pragma once

#include <JuceHeader.h>
#include "Internal/InternalPluginTypes.h"
#include <map>
#include <optional>
#include <vector>

namespace resonance::audio
{
enum class VibeMixTargetKind
{
    Master,
    Drums,
    Vocals,
    Synths,
    FullMix,
    Channel
};

struct VibeMixTarget
{
    VibeMixTargetKind kind = VibeMixTargetKind::Master;
    int channelIndex = 0;

    juce::String displayName() const;
    /** Default mixer channel for Drums / Vocals / Synths group targets. */
    int resolvedChannelIndex() const noexcept;
    bool usesMasterBus() const noexcept;
};

/** One internal plug-in + parameter snapshot in the vibe chain. */
struct VibeSlotPreset
{
    int slotIndex = 0;
    internal::MixPluginId plugin = internal::MixPluginId::Gain;
    std::map<juce::String, float> parameters;
    juce::String rationale;
};

struct VibeMixRecipe
{
    juce::String title;
    juce::String explanation;
    VibeMixTarget target;
    std::vector<VibeSlotPreset> slots;
    std::vector<juce::String> conflicts;
    int schemaVersion = 1;
};

struct VibeMixInterpretResult
{
    bool success = false;
    juce::String error;
    VibeMixRecipe recipe;
    std::optional<VibeMixRecipe> brighterVariant;
    std::optional<VibeMixRecipe> darkerVariant;
};
} // namespace resonance::audio
