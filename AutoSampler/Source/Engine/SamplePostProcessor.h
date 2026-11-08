#pragma once

#include "MultisampleProgram.h"

namespace autosampler
{

struct PostProcessConfig
{
    bool trimSilence = true;
    bool normalize = true;
    bool findSustainLoop = true;
    float trimThresholdDb = -48.0f;
    float trimPadMs = 5.0f;
    float normalizePeakDb = -0.3f;
    float minLoopSeconds = 0.08f;
    float sustainLevelRatio = 0.25f;
    float minLoopConfidence = 0.35f;
    int loopCrossfadeSamples = 64;
};

/** Trim, normalize, and detect zero-cross sustain loops on captured zones. */
class SamplePostProcessor
{
public:
    static void processZone (Zone& zone, const PostProcessConfig& config);

private:
    static juce::AudioBuffer<float> toMono (const juce::AudioBuffer<float>& src);
    static void trimSilence (Zone& zone, const PostProcessConfig& config);
    static void normalizePeak (Zone& zone, const PostProcessConfig& config);
    static void findZeroCrossLoop (Zone& zone, const PostProcessConfig& config);
};

} // namespace autosampler
