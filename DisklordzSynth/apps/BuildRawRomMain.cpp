#include "disklordz/RawRomBuilder.h"

#include <cstdlib>
#include <iostream>

int main (int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: DisklordzSynth_BuildRawRom <output.dlrrom>\n";
        return EXIT_FAILURE;
    }

    if (! disklordz::synth::buildAndWriteRawRom (argv[1]))
    {
        std::cerr << "Failed to build raw ROM\n";
        return EXIT_FAILURE;
    }

    std::cout << "Wrote raw ROM to " << argv[1] << '\n';
    return EXIT_SUCCESS;
}
