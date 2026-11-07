#pragma once

#include <JuceHeader.h>

namespace autosampler
{

/** Writes an overnight mega-bank JSON template (no VST3 hosting required). */
bool writeMegaBankJsonTemplate (const juce::File& jsonFile, juce::String& error);

} // namespace autosampler
