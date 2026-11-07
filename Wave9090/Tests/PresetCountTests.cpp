#include "Presets/FactoryPresets.h"

#include <cassert>
#include <cstdlib>
#include <iostream>

int main()
{
    const auto& presets = wave9090::presets::getFactoryPresets();
    if (presets.size() != wave9090::presets::kFactoryPresetCount || presets.size() != 500)
    {
        std::cerr << "Expected 500 WAVE-9090 presets, got " << presets.size() << '\n';
        return EXIT_FAILURE;
    }

    if (presets.front().name.empty())
    {
        std::cerr << "First preset name is empty\n";
        return EXIT_FAILURE;
    }

    std::cout << "WAVE-9090 preset count OK: " << presets.size() << '\n';
    return EXIT_SUCCESS;
}
