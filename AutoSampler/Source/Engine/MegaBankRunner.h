#pragma once

#include "../AutoSamplerConstants.h"
#include "../Export/MultisampleExporter.h"
#include "Vst3OfflineRenderer.h"

#include <JuceHeader.h>

namespace autosampler
{

struct MegaBankJob
{
    juce::String name { "MegaBank" };
    AutoRecordEngine::Config captureDefaults {};
    std::vector<Vst3CaptureSpec> sources;
};

/** Parse and run multi-VST capture/export (up to 26 sources). */
class MegaBankRunner
{
public:
    static constexpr const char* kFormat = "instruments-autosampler-mega-bank-v1";

    static bool loadJob (const juce::File& jsonFile, MegaBankJob& job, juce::String& error);
    static bool runJob (const MegaBankJob& job, const juce::File& outputDirectory, juce::String& error);
};

} // namespace autosampler
