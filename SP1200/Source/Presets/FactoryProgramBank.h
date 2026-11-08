#pragma once

#include "Engine/SamplerEngine.h"

#include <cstddef>
#include <string>
#include <vector>

namespace sp1200::presets
{

inline constexpr std::size_t kFactoryProgramCount = 256;

struct FactoryProgram
{
    std::string name;
    float filterCutoffNorm = 0.72f;
    float filterResonanceNorm = 0.22f;
    float padTuneSemitones[16] {};
    float padDecayNorm[16] {};
    float padLevel[16] {};
};

const std::vector<FactoryProgram>& getFactoryPrograms() noexcept;
void applyFactoryProgram (sp1200::SamplerEngine& engine, int programIndex) noexcept;
const std::string& programNameAt (int programIndex) noexcept;

} // namespace sp1200::presets
