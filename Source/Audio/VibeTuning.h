#pragma once

#include "VibeMixingTypes.h"

namespace resonance::audio
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
    float width = 1.0f;
    float tapeDrive = 0.0f;
    float hpfHz = 0.0f;

    bool useEq = true;
    bool useComp = true;
    bool useLimiter = false;
    bool useGain = false;
    bool usePan = false;
    bool useWidth = false;
    bool useTape = false;
    bool useHpf = false;
    bool useDeEsser = false;

    juce::StringArray rationales;
};

void applyKeywordInfluences(const juce::String& brief, VibeTuning& tune);
void applyTargetProfile(VibeMixTargetKind target, VibeTuning& tune);
juce::StringArray detectConflicts(const juce::String& brief);
VibeMixRecipe buildRecipeFromTuning(const juce::String& title, const VibeMixTarget& target, const VibeTuning& tune);
} // namespace resonance::audio
