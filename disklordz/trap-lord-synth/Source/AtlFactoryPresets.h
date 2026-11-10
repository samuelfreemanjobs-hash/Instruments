#pragma once
#include <JuceHeader.h>

struct AtlFactoryPresets
{
    static constexpr int kPresetCount = 18;

    enum Bank : int
    {
        churchKeys = 0,
        darkBrass = 1,
        trapOrchestra = 2
    };

    static const juce::StringArray& names();
    static Bank bankForIndex (int index);
    static void apply (juce::AudioProcessorValueTreeState& state, int index);
};
