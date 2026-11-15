#pragma once

#include "../Engine/MultisampleProgram.h"

#include <JuceHeader.h>

namespace autosampler
{

class Exs24Exporter
{
public:
    static bool exportInstrument (const MultisampleProgram& program, const juce::File& destinationDir, juce::String& error);
};

} // namespace autosampler
