#include "Assets/SampleBank.h"
#include "Engine/RomplerEngine.h"
#include "Engine/RomplerParams.h"

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

namespace
{
bool loadPackFile (const char* path, std::vector<std::uint8_t>& out)
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
    disklordz::rompler::assets::SampleBank bank;

    const char* packPath =
#if defined (DLROM_TEST_PACK)
        DLROM_TEST_PACK;
#else
        nullptr;
#endif

    if (argc >= 2)
        packPath = argv[1];

    if (packPath == nullptr)
    {
        std::cerr << "No pack path (build with DLROM_TEST_PACK or pass argv[1])\n";
        return EXIT_FAILURE;
    }

    std::vector<std::uint8_t> bytes;
    if (! loadPackFile (packPath, bytes))
    {
        std::cerr << "Failed to read pack: " << packPath << '\n';
        return EXIT_FAILURE;
    }
    if (! bank.loadEmbedded (bytes.data(), bytes.size()))
    {
        std::cerr << "Failed to parse pack\n";
        return EXIT_FAILURE;
    }

    if (bank.getRegionCount() < 8)
    {
        std::cerr << "Expected >= 8 regions, got " << bank.getRegionCount() << '\n';
        return EXIT_FAILURE;
    }

    disklordz::rompler::engine::RomplerEngine engine;
    engine.setSampleBank (&bank);
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

    std::cout << "DisklordzRomplerTests OK regions=" << bank.getRegionCount() << " peak=" << peak << '\n';
    return EXIT_SUCCESS;
}
