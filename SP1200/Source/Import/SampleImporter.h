#pragma once

#include "../Memory/SampleMemoryPool.h"

#include <juce_audio_formats/juce_audio_formats.h>

#include <string>

namespace sp1200
{

struct ImportResult
{
    SampleSegment segment;
    bool ok = false;
    std::string error;
};

/** Decode file or buffer → mono sum → 26.040 kHz → 12-bit segment (not yet in pool). */
class SampleImporter
{
public:
    SampleImporter();

    ImportResult importFromFile (const juce::File& file,
                                 int bankIndex,
                                 juce::String segmentName,
                                 bool vinylRateTrick = false);

    ImportResult importFromAudioBuffer (const juce::AudioBuffer<float>& buffer,
                                        double sourceSampleRate,
                                        int bankIndex,
                                        juce::String segmentName,
                                        bool vinylRateTrick = false);

private:
    juce::AudioFormatManager formatManager_;
};

} // namespace sp1200
