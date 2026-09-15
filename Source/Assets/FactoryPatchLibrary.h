#pragma once

#include "DSP/Modulation/ToneCoupling.h"

#include <array>
#include <cstddef>
#include <cstdint>

namespace jdupgraded::assets
{

inline constexpr std::size_t kFactoryPatchCount = 128;

struct FactoryToneLayer final
{
    std::uint16_t multisampleSetId;
    std::uint16_t waveIndex;
    float level;
    float coarseSemis;
    float filterCutoffNorm;
    float filterResonanceNorm;
};

struct FactoryPatch final
{
    const char* name;
    std::array<FactoryToneLayer, 4> tones;
    jdupgraded::dsp::ToneCouplingMode coupling;
    float masterGain;
    float groupADrive;
    float groupBMix;
};

class FactoryPatchLibrary final
{
public:
    static std::size_t getPatchCount() noexcept;
    static const FactoryPatch& getPatch (std::size_t index) noexcept;
};

} // namespace jdupgraded::assets
