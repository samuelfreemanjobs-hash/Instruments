#pragma once

#include "Presets/FactoryPresets.h"

namespace prophetrev2::presets
{

/** Builds the 1,028-preset factory library from 21 foundations + procedural variations. */
class PresetFactory
{
public:
    static std::vector<FactoryPreset> buildLibrary();
};

} // namespace prophetrev2::presets
