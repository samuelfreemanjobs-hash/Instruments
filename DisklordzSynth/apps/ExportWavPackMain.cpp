#include "disklordz/FactoryPackBuilder.h"
#include "disklordz/WavWriter.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

int main (int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: DisklordzSynth_ExportWavPack <output_directory> [sampleRate]\n"
                  << "Exports all factory multisample regions as sellable WAV files.\n";
        return EXIT_FAILURE;
    }

    const fs::path outDir (argv[1]);
    const std::uint32_t sr = argc > 2 ? static_cast<std::uint32_t> (std::stoul (argv[2])) : 48000u;

    std::error_code ec;
    fs::create_directories (outDir, ec);
    if (ec)
    {
        std::cerr << "Could not create directory\n";
        return EXIT_FAILURE;
    }

    std::vector<disklordz::synth::PackRegionSpec> specs;
    disklordz::synth::buildRomplerFactoryRegions (specs);

    int written = 0;
    for (std::size_t i = 0; i < specs.size(); ++i)
    {
        const auto& s = specs[i];
        const std::string fname = "tone" + std::to_string (s.toneIndex)
                                + "_root" + std::to_string (s.root)
                                + "_cat" + std::to_string (static_cast<int> (s.category))
                                + ".wav";
        const fs::path path = outDir / fname;
        if (disklordz::synth::writeMonoWav24 (path.string(), s.pcm, sr))
            ++written;
    }

    std::cout << "Exported " << written << " WAVs to " << outDir << '\n';
    return written > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
