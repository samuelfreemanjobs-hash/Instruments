#pragma once

#include "TrackModel.h"
#include <JuceHeader.h>

namespace huckaby
{

/** Scan a folder for .mid/.midi, apply pattern at index to bits + per-step velocities. */
bool applyMidiPatternFromFolder(const juce::File& folder,
                                int patternIndex,
                                uint64_t& outBits,
                                std::array<float, kMaxSteps>& outVelocities,
                                juce::String& outError);

/** Parse one SMF; map notes onto 16th-note steps (first bar), channel-agnostic. */
bool importPatternFromMidiFile(const juce::File& file,
                               uint64_t& outBits,
                               std::array<float, kMaxSteps>& outVelocities,
                               juce::String& outError);

/** Write current pattern as a one-bar SMF (grid 16ths, 480 PPQ). */
bool exportPatternToMidiFile(const juce::File& file,
                             const uint64_t bits,
                             const std::array<float, kMaxSteps>& velocities,
                             int numSteps,
                             juce::String& outError);

/** Create default pattern MIDIs under …/HuckabyPatternEngine/MidiPatterns/track_N/ (once). */
void ensureFactoryMidiPatternsOnDisk();

juce::File getDefaultMidiPatternsRoot();

} // namespace huckaby
