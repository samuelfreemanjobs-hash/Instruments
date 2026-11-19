#pragma once

#include "Presets/FactoryPresets.h"

#include <vector>

namespace wave9090::presets
{

/** Presets promoted by daily VST Preset Agent batches (lineage DB). */
std::vector<FactoryPreset> getActiveAgentSeeds();

} // namespace wave9090::presets
