#include "Presets/FactoryPresets.h"
#include "Presets/PresetFactory.h"

#include <iostream>

int runPresetLibraryTests()
{
    const auto& lib = prophetrev2::presets::getFactoryPresets();
    if (static_cast<int> (lib.size()) != prophetrev2::presets::kFactoryPresetCount)
    {
        std::cerr << "Expected " << prophetrev2::presets::kFactoryPresetCount << " presets, got " << lib.size() << "\n";
        return 1;
    }

    int foundations = 0;
    for (const auto& p : lib)
        if (p.isFoundation)
            ++foundations;

    if (foundations != prophetrev2::presets::kFoundationPresetCount)
    {
        std::cerr << "Foundation count mismatch\n";
        return 1;
    }

    const auto rebuilt = prophetrev2::presets::PresetFactory::buildLibrary();
    if (lib[500].name != rebuilt[500].name)
    {
        std::cerr << "Preset factory is not deterministic\n";
        return 1;
    }

    return 0;
}
