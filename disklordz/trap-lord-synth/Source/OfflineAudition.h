#pragma once
#include <JuceHeader.h>
#include <vector>

struct OfflineAudition
{
    static constexpr double kBpm = 140.;
    static constexpr double kDurationSec = 8.;
    static constexpr double kSampleRate = 44100.;

    /** Category-aware phrase MIDI (pads/swells = held minor chord, leads/keys = melody, brass/hits = stabs, bass = long 808). */
    static std::vector<std::pair<int, juce::MidiMessage>> buildTimeline (const juce::String& category,
                                                                        bool includeSwellModWheel);

    static juce::String safeFilename (const juce::String& category, const juce::String& presetName, int slot);
};
