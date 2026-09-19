#pragma once

#include "SynthParameterIds.h"
#include "SynthProcessor.h"

namespace gfunk
{
enum class PresetId : int
{
    gFunkLead = 0,
    gFunkBass = 1,
};

void applyPreset(MoogVoyagerAudioProcessor& processor, PresetId preset);
} // namespace gfunk
