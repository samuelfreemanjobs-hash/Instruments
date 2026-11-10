#pragma once
#include <JuceHeader.h>
#include <vector>

struct MpcSampleLayer
{
    int velLow = 1;
    int velHigh = 127;
    int rootNote = 60;
    juce::String sampleName;
    int sampleFrames = 0;
    int loopStart = 0;
    int loopEnd = 0;
};

/** One MPC keygroup = one Instrument with up to 4 velocity layers. */
struct MpcKeygroupZone
{
    int lowNote = 0;
    int highNote = 127;
    int rootNote = 60;
    float volumeAttack = 0.f;
    float volumeDecay = 0.f;
    float volumeSustain = 1.f;
    float volumeRelease = 0.35f;
    int loopStart = 0;
    int loopEnd = 0;
    std::vector<MpcSampleLayer> layers;
};

class MpcXpmWriter
{
public:
    static constexpr const char* kFileVersion = "5000";

    static bool writeKeygroupProgram (const juce::File& programFolder,
                                      const juce::String& programName,
                                      const std::vector<MpcKeygroupZone>& keygroups,
                                      float pitchBendRange = 2.f);
    static juce::String makeSafeName (const juce::String& input, int maxLen = 32);
    static juce::String makeSampleFilename (int rootNote, int layerIndex);
};
