#pragma once
#include <JuceHeader.h>
#include <vector>

struct OfflineAudition
{
    static constexpr double kBpm = 140.;
    static constexpr double kDurationSec = 8.;
    static constexpr double kSampleRate = 44100.;

    /** Minor-chord phrase + F harmonic minor melody (see tools/write_demo_midi.py). */
    static std::vector<std::pair<int, juce::MidiMessage>> buildTimeline (bool includeSwellModWheel);

    static juce::String safeFilename (const juce::String& category, const juce::String& presetName, int slot);
};
