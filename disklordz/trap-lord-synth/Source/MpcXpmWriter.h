#pragma once
#include <JuceHeader.h>
#include <vector>

struct MpcSampleZone
{
    int rootNote = 60;
    int lowNote = 0;
    int highNote = 127;
    int velLow = 1;
    int velHigh = 127;
    juce::String sampleName;
    int sampleFrames = 0;
};

class MpcXpmWriter
{
public:
  /** Writes `<programName>.xpm` into `programFolder` (zones reference uppercase .WAV names). */
    static bool writeKeygroupProgram (const juce::File& programFolder,
                                    const juce::String& programName,
                                    const std::vector<MpcSampleZone>& zones);
    static juce::String makeSafeName (const juce::String& input, int maxLen = 48);
};
