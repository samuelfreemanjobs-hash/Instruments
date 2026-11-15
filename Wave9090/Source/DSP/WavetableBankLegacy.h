#pragma once

#include "DSP/WavetableBank.h"

namespace wave9090::dsp
{

inline float legacySampleMorph (const WavetableBank& bank, double phase01, float positionNorm, double phaseInc) noexcept
{
    return bank.sampleLegacyMorph (phase01, positionNorm, phaseInc);
}

} // namespace wave9090::dsp