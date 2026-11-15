#pragma once

#include "../AutoSamplerConstants.h"
#include "AutoRecordEngine.h"
#include "MultisampleProgram.h"

namespace autosampler
{
class Vst3PluginCache;
}

#include <JuceHeader.h>

namespace autosampler
{

struct Vst3CaptureSpec
{
    juce::File vst3Path;
    juce::String presetName;
    int pluginProgramIndex = 0;
    AutoRecordEngine::Config captureConfig {};
};

/** Offline multisample capture by hosting a source VST3 (desktop hosts only). */
class Vst3OfflineRenderer
{
public:
    static bool isSupportedOnThisPlatform() noexcept;

    /** Renders each note in captureConfig; applies post-process identically to live auto-sample. */
    static bool captureProgram (const Vst3CaptureSpec& spec,
                                MultisampleProgram& dest,
                                juce::String& error,
                                Vst3PluginCache* cache = nullptr,
                                double timeoutSeconds = 1800.0);
};

} // namespace autosampler
