#include "disklordz/MultisampleRomBuilder.h"
#include "disklordz/RawRomBuilder.h"
#include "disklordz/RomFactorySpec.h"
#include "disklordz/content/ContentValidation.h"
#include "disklordz/content/ExpansionManifest.h"

#include <cstdlib>
#include <iostream>
#include <string>

int main (int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "DISKLORDZ ROM FACTORY\n\n"
                  << "Usage:\n"
                  << "  DisklordzSynth_RomFactory build-procedural <output_dir>\n"
                  << "  DisklordzSynth_RomFactory build-manifest <manifest_dir> <out.dlrrom> [bank_index]\n"
                  << "  DisklordzSynth_RomFactory print-targets\n"
                  << "  DisklordzSynth_RomFactory validate-package <expansion_or_package_dir>\n";
        return EXIT_FAILURE;
    }

    const std::string cmd = argv[1];

    if (cmd == "print-targets")
    {
        std::cout << "Layers: " << disklordz::factory::kRomLayerCount << " (REAL, CLASSIC_DIGITAL, ANALOG, LO_FI, CYBER_SHIFT)\n"
                  << "Families: " << static_cast<int> (disklordz::factory::InstrumentFamily::count) << "\n"
                  << "Source multis target: " << disklordz::factory::kTargetSourceMultisamplesMin << "–"
                  << disklordz::factory::kTargetSourceMultisamplesMax << "\n"
                  << "Raw WAV target: " << disklordz::factory::kTargetRawWavMin << "–"
                  << disklordz::factory::kTargetRawWavMax << "\n"
                  << "Factory preset target: " << disklordz::factory::kTargetFactoryPresetsMin << "+\n";
        return EXIT_SUCCESS;
    }

    if (cmd == "build-procedural")
    {
        if (argc < 3)
            return EXIT_FAILURE;
        const int n = disklordz::synth::buildAndWriteAllFactoryRomBanks (argv[2]);
        std::cout << "Wrote " << n << " procedural PCM cards\n";
        return n > 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    if (cmd == "build-manifest")
    {
        if (argc < 4)
            return EXIT_FAILURE;
        const std::uint32_t bank = argc > 4 ? static_cast<std::uint32_t> (std::stoul (argv[4])) : 0u;
        std::string error;
        if (! disklordz::factory::buildRomCardFromManifests (argv[2], argv[3], bank, error))
        {
            std::cerr << error << '\n';
            return EXIT_FAILURE;
        }
        std::cout << "Wrote manifest ROM " << argv[3] << '\n';
        return EXIT_SUCCESS;
    }

    if (cmd == "validate-package")
    {
        if (argc < 3)
            return EXIT_FAILURE;
        disklordz::content::ContentManifest manifest;
        std::string error;
        const std::string manifestPath = std::string (argv[2]) + "/manifest.json";
        if (! disklordz::content::loadContentManifestJson (manifestPath, manifest, error))
        {
            std::cerr << error << '\n';
            return EXIT_FAILURE;
        }
        const auto report = disklordz::content::validatePackageManifest (manifest);
        if (! report.ok)
        {
            std::cerr << report.message << '\n';
            return EXIT_FAILURE;
        }
        std::cout << "Package OK: " << manifest.contentId << " v" << manifest.version << '\n';
        return EXIT_SUCCESS;
    }

    std::cerr << "Unknown command: " << cmd << '\n';
    return EXIT_FAILURE;
}
