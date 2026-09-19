#include "Assets/RawRomBank.h"
#include "Engine/RomplerEngine.h"
#include "Engine/RomplerParams.h"

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

namespace
{
bool loadRomFile (const fs::path& path, std::vector<std::uint8_t>& out)
{
    std::ifstream in (path, std::ios::binary);
    if (! in)
        return false;
    in.seekg (0, std::ios::end);
    const auto size = in.tellg();
    in.seekg (0, std::ios::beg);
    out.resize (static_cast<std::size_t> (size));
    in.read (reinterpret_cast<char*> (out.data()), size);
    return in.good();
}

bool loadLibraryFromDirectory (const fs::path& dir, disklordz::rompler::assets::RawRomLibrary& library)
{
    if (! fs::is_directory (dir))
        return false;

    int loaded = 0;
    for (const auto& entry : fs::directory_iterator (dir))
    {
        if (entry.path().extension() != ".dlrrom")
            continue;
        std::vector<std::uint8_t> bytes;
        if (! loadRomFile (entry.path(), bytes))
            continue;
        if (library.loadBankEmbedded (bytes.data(), bytes.size()))
            ++loaded;
    }
    return loaded > 0;
}
} // namespace

int main (int argc, char** argv)
{
    disklordz::rompler::assets::RawRomLibrary library;

    fs::path romDir =
#if defined (DLRROM_TEST_ROM_DIR)
        DLRROM_TEST_ROM_DIR;
#else
        fs::path {};
#endif

    if (argc >= 2)
        romDir = argv[1];

    if (romDir.empty() || ! loadLibraryFromDirectory (romDir, library))
    {
        std::cerr << "Failed to load ROM library from directory (use DLRROM_TEST_ROM_DIR or argv[1])\n";
        return EXIT_FAILURE;
    }

    if (library.getBankCount() < 1)
    {
        std::cerr << "No ROM banks loaded\n";
        return EXIT_FAILURE;
    }

    if (library.getTotalWaveCount() < 400)
    {
        std::cerr << "Expected abundant wave pool (>=400), got " << library.getTotalWaveCount() << '\n';
        return EXIT_FAILURE;
    }

    disklordz::rompler::engine::RomplerEngine engine;
    engine.setRomLibrary (&library);
    engine.prepare (48000.0);

    disklordz::rompler::RomplerParams p;
    p.toneLevel = { 1.0f, 0.6f, 0.4f, 0.8f };
    p.toneRomBank = { 0, 0, 0, 0 };
    p.toneProgram = { 0, 1, 2, 0 };
    p.ampSustain = 0.9f;
    engine.setParams (p);
    engine.noteOn (60, 1.0f);

    std::vector<float> left (48000, 0.0f);
    std::vector<float> right (48000, 0.0f);
    engine.render (left.data(), right.data(), static_cast<int> (left.size()));

    float peak = 0.0f;
    for (float s : left)
        peak = std::max (peak, std::abs (s));

    if (peak < 1.0e-4f)
    {
        std::cerr << "Render peak too low: " << peak << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "DisklordzRomplerTests OK banks=" << library.getBankCount()
              << " waves=" << library.getTotalWaveCount() << " peak=" << peak << '\n';
    return EXIT_SUCCESS;
}
