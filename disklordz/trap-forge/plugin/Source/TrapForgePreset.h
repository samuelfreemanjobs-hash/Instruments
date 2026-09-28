#pragma once

#include <JuceHeader.h>

namespace trapforge
{

enum class DrumCategory
{
    kick,
    sub808,
    snare,
    clap,
    hihat,
    perc
};

struct TrapForgePreset
{
    juce::String name;
    DrumCategory category = DrumCategory::kick;
    float rootHz = 50.0f;
    float pitchMod = 36.0f;
    float pitchDecay = 0.022f;
    float glideMs = 0.0f;
    float glideSemi = 0.0f;
    float glideExponent = 2.2f;
    float ampAttack = 0.001f;
    float ampDecay = 0.12f;
    float ampSustain = 0.0f;
    float ampRelease = 0.05f;
    float drive = 0.4f;
    float outputCeilingDb = -0.3f;
};

DrumCategory categoryFromString (const juce::String& cat);
TrapForgePreset presetFromJson (const juce::var& json);
juce::Array<TrapForgePreset> loadFactoryPresets();

} // namespace trapforge
