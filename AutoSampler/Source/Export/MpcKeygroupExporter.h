#pragma once

#include "../Engine/MultisampleProgram.h"

#include <JuceHeader.h>

namespace autosampler
{

/**
 * Akai MPC keygroup programs (.xpm XML + companion .wav files) for MPC Live/One/X/Key firmware 2.x.
 * XPM v2 field names and keyzone splitting match github.com/samuelfreemanjobs-hash/push PR #8.
 */
class MpcKeygroupExporter
{
public:
    static constexpr const char* kFileVersion = "2.1";
    static constexpr const char* kAppVersion = "2.10.0";

    static juce::String mpcSampleBaseName (int rootNote, int velStart = 0);
    static juce::String escapeXml (const juce::String& text);

    /** Writes `<name>/<name>.xpm` and uppercase `.WAV` samples in destinationDir. */
    static bool exportKeygroup (const MultisampleProgram& program, const juce::File& destinationDir, juce::String& error);
};

} // namespace autosampler
