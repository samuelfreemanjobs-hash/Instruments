#include "disklordz/EngineParams.h"
#include "disklordz/FactoryPackBuilder.h"
#include "disklordz/ProceduralSynth.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace
{
float peakOf (const std::vector<float>& v)
{
    float peak = 0.0f;
    for (float s : v)
        peak = std::max (peak, std::abs (s));
    return peak;
}
} // namespace

int main()
{
    disklordz::synth::EngineParams p;
    p.a = 0.5f;
    p.b = 0.4f;

    for (auto id : { disklordz::synth::EngineId::subtractive,
                     disklordz::synth::EngineId::additive,
                     disklordz::synth::EngineId::karplus,
                     disklordz::synth::EngineId::wave })
    {
        const auto buf = disklordz::synth::renderEngine (id, 4096, p);
        const float peak = peakOf (buf);
        if (peak < 0.01f || peak > 1.05f)
        {
            std::cerr << disklordz::synth::engineIdName (id) << " peak out of range: " << peak << '\n';
            return EXIT_FAILURE;
        }
    }

    std::vector<disklordz::synth::PackRegionSpec> specs;
    disklordz::synth::buildRomplerFactoryRegions (specs);
    if (specs.size() != 32)
    {
        std::cerr << "expected 32 regions, got " << specs.size() << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "DisklordzSynthTests OK (4 engines)\n";
    return EXIT_SUCCESS;
}
