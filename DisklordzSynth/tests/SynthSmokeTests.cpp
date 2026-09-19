#include "disklordz/EngineParams.h"
#include "disklordz/ProceduralSynth.h"
#include "disklordz/RawRomBuilder.h"

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

    disklordz::synth::BuiltRawRom rom;
    if (! disklordz::synth::buildRawRom (rom))
    {
        std::cerr << "buildRawRom failed\n";
        return EXIT_FAILURE;
    }

    constexpr std::size_t kExpectedWaves = 4 * 8 + 4 * 4;
    if (rom.waves.size() != kExpectedWaves)
    {
        std::cerr << "expected " << kExpectedWaves << " waves, got " << rom.waves.size() << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "DisklordzSynthTests OK (4 engines, raw ROM " << rom.waves.size() << " waves)\n";
    return EXIT_SUCCESS;
}
