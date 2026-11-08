#pragma once

#include "../Engine/MultisampleProgram.h"

#include <JuceHeader.h>

namespace autosampler
{

struct MpcKeyzoneValidation
{
    bool ok = true;
    juce::StringArray errors;
    juce::String reportText;
};

/** Validates MIDI coverage and velocity spans before MPC export (push#8 parity). */
class MpcKeyzoneValidator
{
public:
    static MpcKeyzoneValidation validateProgram (const MultisampleProgram& program);
};

} // namespace autosampler
