#pragma once

#include "Presets/FactoryPresets.h"

namespace rev2trap::presets
{

constexpr int kNumJz400Presets = 400;
constexpr int kRev2ProgramOffset = 0;
constexpr int kJz400ProgramOffset = kNumFactoryPresets;

/** Programs 128–527 (JZ400 bank). */
SynthParams getJz400Preset (int index0to399) noexcept;

enum class PresetBank { rev2, jz400 };

inline SynthParams getPresetForProgram (int program) noexcept
{
    if (program >= kJz400ProgramOffset)
        return getJz400Preset (program - kJz400ProgramOffset);
    return getFactoryPreset (program);
}

constexpr int kTotalFactoryPrograms = kNumFactoryPresets + kNumJz400Presets;

} // namespace rev2trap::presets
