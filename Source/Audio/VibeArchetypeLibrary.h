#pragma once

#include <JuceHeader.h>

namespace resonance::audio
{
struct VibeTuning;

/** Curated starting points blended with keyword scoring. */
class VibeArchetypeLibrary
{
public:
    static void applyBestArchetype(const juce::String& normalisedBrief, VibeTuning& tune);
    static juce::StringArray chipSuggestions();
};
} // namespace resonance::audio
