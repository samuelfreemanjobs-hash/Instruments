#include "disklordz/RawRomBuilder.h"
#include "disklordz/RawRomFormat.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main (int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage:\n"
                  << "  DisklordzSynth_BuildRawRom <output_directory>\n"
                  << "      Writes " << disklordz::rawrom::kFactoryRomBankCount
                  << " pcm_bank_XX.dlrrom expansion cards.\n"
                  << "  DisklordzSynth_BuildRawRom <output.dlrrom> --single\n"
                  << "      Writes one legacy bank-0 card only.\n";
        return EXIT_FAILURE;
    }

    const fs::path target (argv[1]);
    const bool single = argc >= 3 && std::string (argv[2]) == "--single";

    if (single || target.extension() == ".dlrrom")
    {
        if (! disklordz::synth::buildAndWriteRawRom (target.string()))
        {
            std::cerr << "Failed to build raw ROM\n";
            return EXIT_FAILURE;
        }
        std::cout << "Wrote raw ROM to " << target << '\n';
        return EXIT_SUCCESS;
    }

    const int n = disklordz::synth::buildAndWriteAllFactoryRomBanks (target.string());
    if (n != static_cast<int> (disklordz::rawrom::kFactoryRomBankCount))
    {
        std::cerr << "Expected " << disklordz::rawrom::kFactoryRomBankCount << " ROM files, wrote " << n << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Wrote " << n << " factory PCM ROM cards to " << target << '\n';
    return EXIT_SUCCESS;
}
