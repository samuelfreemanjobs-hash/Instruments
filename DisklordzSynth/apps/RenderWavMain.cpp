#include "disklordz/ProceduralSynth.h"
#include "disklordz/WavWriter.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace
{

disklordz::synth::EngineId parseEngine (const std::string& name)
{
    if (name == "bell") return disklordz::synth::EngineId::bell;
    if (name == "keys") return disklordz::synth::EngineId::keys;
    if (name == "pad") return disklordz::synth::EngineId::pad;
    if (name == "sub808" || name == "sub") return disklordz::synth::EngineId::sub808;
    return disklordz::synth::EngineId::bell;
}

} // namespace

int main (int argc, char** argv)
{
    if (argc < 4)
    {
        std::cerr << "Usage: DisklordzSynth_RenderWav <engine> <frames> <output.wav> [paramA] [paramB] [sampleRate]\n"
                  << "  engine: bell | keys | pad | sub808\n";
        return EXIT_FAILURE;
    }

    const auto engine = parseEngine (argv[1]);
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

    std::cout << "Wrote " << pcm.size() << " frames to " << outPath << " (" << disklordz::synth::engineIdName (engine) << ")\n";
    return EXIT_SUCCESS;
}
