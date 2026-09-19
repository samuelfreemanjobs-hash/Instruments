#include "disklordz/EngineParams.h"
#include "disklordz/ProceduralSynth.h"
#include "disklordz/WavWriter.h"

#include <cstdlib>
#include <iostream>
#include <string>

int main (int argc, char** argv)
{
    if (argc < 4)
    {
        std::cerr << "Usage: DisklordzSynth_RenderWav <engine> <frames> <output.wav> [paramA] [paramB] [sampleRate]\n"
                  << "  engine: subtractive | additive | karplus | wave\n"
                  << "  aliases: sub808, bell, keys, pad, wavetable\n";
        return EXIT_FAILURE;
    }

    const auto engine = disklordz::synth::parseEngineId (argv[1]);
    const std::size_t frames = static_cast<std::size_t> (std::stoul (argv[2]));
    const std::string outPath = argv[3];
    const float paramA = argc > 4 ? std::stof (argv[4]) : 0.5f;
    const float paramB = argc > 5 ? std::stof (argv[5]) : 0.5f;
    const std::uint32_t sr = argc > 6 ? static_cast<std::uint32_t> (std::stoul (argv[6])) : 48000u;

    const auto pcm = disklordz::synth::renderEngine (engine, frames, paramA, paramB);
    if (pcm.empty())
    {
        std::cerr << "Render failed\n";
        return EXIT_FAILURE;
    }

    if (! disklordz::synth::writeMonoWav24 (outPath, pcm, sr))
    {
        std::cerr << "WAV write failed\n";
        return EXIT_FAILURE;
    }

    std::cout << "Wrote " << pcm.size() << " frames to " << outPath << " ("
              << disklordz::synth::engineIdName (engine) << ")\n";
    return EXIT_SUCCESS;
}
