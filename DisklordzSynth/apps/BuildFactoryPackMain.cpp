#include "disklordz/FactoryPackBuilder.h"

#include <cstdlib>
#include <iostream>
#include <vector>

int main (int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: DisklordzSynth_BuildFactoryPack <output.dlrom>\n";
        return EXIT_FAILURE;
    }

    std::vector<disklordz::synth::PackRegionSpec> specs;
    disklordz::synth::buildRomplerFactoryRegions (specs);

    if (! disklordz::synth::writeDlromPack (argv[1], specs))
    {
        std::cerr << "Failed to write pack\n";
        return EXIT_FAILURE;
    }

    std::cout << "Wrote " << specs.size() << " regions to " << argv[1] << '\n';
    return EXIT_SUCCESS;
}
