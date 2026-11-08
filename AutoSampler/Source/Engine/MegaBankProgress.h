#pragma once

#include <JuceHeader.h>

namespace autosampler
{

struct MegaBankRunOptions
{
    /** No retries between attempts when true (default). */
    bool failFast = true;
    /** Keep processing remaining VST3s after a per-source failure. */
    bool skipFailedSources = true;
    int maxRetriesPerSource = 0;
    double sourceTimeoutSeconds = 1800.0;
    bool resume = true;
    bool writeProgress = true;
    juce::File pluginCacheFile;
};

/** On-disk resume + progress for overnight mega-bank jobs. */
class MegaBankProgress
{
public:
    static constexpr const char* kFormat = "instruments-autosampler-mega-bank-progress-v1";

    static juce::File progressFileFor (const juce::File& outputRoot, const juce::String& bankName);
    static juce::File validationReportFor (const juce::File& outputRoot, const juce::String& bankName);
    static juce::File skippedReportFor (const juce::File& outputRoot, const juce::String& bankName);

    static bool load (const juce::File& file, juce::var& state, juce::String& error);
    static bool save (const juce::File& file, const juce::var& state, juce::String& error);

    static bool isSourceCompleted (const juce::var& state, const juce::String& presetName);
    static void markSourceCompleted (juce::var& state, const juce::String& presetName, const juce::File& exportRel);
    static void markSourceFailed (juce::var& state, const juce::String& presetName, const juce::String& err, int attempts);
    static void setRunning (juce::var& state, const juce::String& bankName, int totalSources, const juce::String& current);
    static void setFinished (juce::var& state, bool success, bool hadSkips = false);

    static juce::var createEmpty (const juce::String& bankName, int totalSources);
};

} // namespace autosampler
