#include "disklordz/FactoryPackBuilder.h"
#include "disklordz/ProceduralSynth.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

int main()
{
    auto bell = disklordz::synth::renderBell (2048, 0.5f, 4.0f);
    float peak = 0.0f;
    for (float s : bell)
        peak = std::max (peak, std::abs (s));
    if (peak < 0.01f || peak > 1.0f)
    {
        std::cerr << "bell peak out of range: " << peak << '\n';
        return EXIT_FAILURE;
    }

    std::vector<disklordz::synth::PackRegionSpec> specs;
    disklordz::synth::buildRomplerFactoryRegions (specs);
    if (specs.size() != 32)
    {
        std::cerr << "expected 32 regions, got " << specs.size() << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "DisklordzSynthTests OK\n";
    return EXIT_SUCCESS;
}
