#include "Presets/FactoryPresets.h"

#include "Presets/PresetFactory.h"

namespace prophetrev2::presets
{

const std::vector<FactoryPreset>& getFactoryPresets() noexcept
{
    static const std::vector<FactoryPreset> kLibrary = PresetFactory::buildLibrary();
    return kLibrary;
}

} // namespace prophetrev2::presets
