#pragma once

#include <JuceHeader.h>

namespace autosampler
{

/** Remembers last-known-good VST3 paths to speed mega-bank reruns. */
class Vst3PluginCache
{
public:
    explicit Vst3PluginCache (juce::File cacheFile = {});

    void setCacheFile (const juce::File& file);
    bool wasLastLoadOk (const juce::File& vst3Path) const;
    void markOk (const juce::File& vst3Path);
    void markFailed (const juce::File& vst3Path);

private:
    void loadFromDisk();
    void saveToDisk() const;

    juce::File cacheFile_;
    juce::var state_;
};

} // namespace autosampler
