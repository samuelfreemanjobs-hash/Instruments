#include "disklordz/EngineParams.h"
#include "disklordz/ProceduralSynth.h"
#include "disklordz/RawRomBuilder.h"
#include "disklordz/RawRomFormat.h"

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

constexpr std::size_t kExpectedWavesPerBank =
    disklordz::rawrom::kProgramsPerRomBank * disklordz::rawrom::kZonesPerProgram
    + disklordz::rawrom::kStandaloneWavesPerBank;
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
    if (! disklordz::synth::buildRawRomBank (0, rom))
    {
        std::cerr << "buildRawRomBank failed\n";
        return EXIT_FAILURE;
    }

    if (rom.waves.size() != kExpectedWavesPerBank)
    {
        std::cerr << "expected " << kExpectedWavesPerBank << " waves per ROM card, got " << rom.waves.size() << '\n';
        return EXIT_FAILURE;
    }

    if (disklordz::rawrom::kFactoryRomBankCount < 8)
    {
        std::cerr << "factory ROM bank count too low\n";
        return EXIT_FAILURE;
    }

    std::cout << "DisklordzSynthTests OK (4 engines, " << disklordz::rawrom::kFactoryRomBankCount
              << " ROM cards × " << kExpectedWavesPerBank << " waves/card design)\n";
    return EXIT_SUCCESS;
}
