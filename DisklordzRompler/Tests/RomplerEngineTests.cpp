#include "Assets/RawRomBank.h"
#include "Engine/RomplerEngine.h"
#include "Engine/RomplerParams.h"

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

namespace
{
bool loadRomFile (const char* path, std::vector<std::uint8_t>& out)
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
} // namespace

int main (int argc, char** argv)
{
    disklordz::rompler::assets::RawRomBank bank;

    const char* romPath =
#if defined (DLRROM_TEST_ROM)
        DLRROM_TEST_ROM;
#else
        nullptr;
#endif

    if (argc >= 2)
        romPath = argv[1];

    if (romPath == nullptr)
    {
        std::cerr << "No ROM path (build with DLRROM_TEST_ROM or pass argv[1])\n";
        return EXIT_FAILURE;
    }

    std::vector<std::uint8_t> bytes;
    if (! loadRomFile (romPath, bytes))
    {
        std::cerr << "Failed to read ROM: " << romPath << '\n';
        return EXIT_FAILURE;
    }
    if (! bank.loadEmbedded (bytes.data(), bytes.size()))
    {
        std::cerr << "Failed to parse DLRROM01\n";
        return EXIT_FAILURE;
    }

    if (bank.getWaveCount() < 32)
    {
        std::cerr << "Expected >= 32 waves in ROM, got " << bank.getWaveCount() << '\n';
        return EXIT_FAILURE;
    }

    disklordz::rompler::engine::RomplerEngine engine;
    engine.setRomBank (&bank);
    engine.prepare (48000.0);

    disklordz::rompler::RomplerParams p;
    p.toneLevel = { 1.0f, 0.6f, 0.4f, 0.8f };
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

    std::cout << "DisklordzRomplerTests OK waves=" << bank.getWaveCount() << " peak=" << peak << '\n';
    return EXIT_SUCCESS;
}
