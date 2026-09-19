#include "disklordz/EngineParams.h"
#include "disklordz/RawRomBuilder.h"
#include "disklordz/RawRomFormat.h"
#include "disklordz/WavWriter.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

int main (int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: DisklordzSynth_ExportWavPack <output_directory> [sampleRate]\n"
                  << "Exports all waves from every factory PCM ROM card as sellable WAV files.\n";
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

    int written = 0;
    for (std::uint32_t bank = 0; bank < disklordz::rawrom::kFactoryRomBankCount; ++bank)
    {
        disklordz::synth::BuiltRawRom built;
        if (! disklordz::synth::buildRawRomBank (bank, built))
            continue;

        for (std::size_t i = 0; i < built.waves.size(); ++i)
        {
            const auto& e = built.waves[i];
            const std::size_t end = static_cast<std::size_t> (e.pcmOffset + e.numFrames);
            if (end > built.pcm.size())
                continue;

            std::vector<float> mono (e.numFrames);
            for (std::uint32_t f = 0; f < e.numFrames; ++f)
                mono[f] = built.pcm[static_cast<std::size_t> (e.pcmOffset + f)];

            const std::string eng = disklordz::synth::engineIdName (
                static_cast<disklordz::synth::EngineId> (e.engineId));
            const std::string fname = "bank" + std::to_string (bank) + "_wave" + std::to_string (e.waveId) + "_"
                                    + eng + "_set" + std::to_string (e.multisampleSetId) + "_root"
                                    + std::to_string (e.rootMidiNote) + ".wav";
            const fs::path path = outDir / fname;
            if (disklordz::synth::writeMonoWav24 (path.string(), mono, sr))
                ++written;
        }
    }

    std::cout << "Exported " << written << " WAVs to " << outDir << '\n';
    return written > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
