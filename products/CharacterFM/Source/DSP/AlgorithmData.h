#pragma once

#include "ModulationGraph.h"

namespace characterfm::dsp
{

/** Returns DX7-style routing for algorithm index 0..31. */
const ModulationGraph& getAlgorithmGraph (int algorithmIndex) noexcept;

} // namespace characterfm::dsp
