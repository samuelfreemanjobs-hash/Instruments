#include "Assets/PackFormat.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

namespace
{
constexpr double kPi = 3.14159265358979323846;

float clampf (float v, float lo, float hi)
{
    return std::max (lo, std::min (hi, v));
}

void normalizePeak (std::vector<float>& buffer, float target = 0.92f)
{
    float peak = 0.0f;
    for (float s : buffer)
        peak = std::max (peak, std::abs (s));
    if (peak > 1.0e-6f)
    {
        const float g = target / peak;
        for (float& s : buffer)
            s *= g;
    }
}

std::vector<float> renderBell (std::size_t frames, float inharmonicity, float decay)
{
    std::vector<float> out (frames, 0.0f);
    const double partials[] = { 1.0, 2.12, 3.25, 4.55, 5.9 };
    const double gains[] = { 1.0, 0.5, 0.32, 0.2, 0.12 };
    for (std::size_t i = 0; i < frames; ++i)
    {
        const double t = static_cast<double> (i) / static_cast<double> (frames);
        double sum = 0.0;
        for (int p = 0; p < 5; ++p)
        {
            const double f = partials[p] * (1.0 + inharmonicity * 0.015 * static_cast<double> (p));
            sum += gains[p] * std::sin (2.0 * kPi * f * t) * std::exp (-decay * t * (1.0 + 0.25 * p));
        }
        out[i] = static_cast<float> (sum);
    }
    normalizePeak (out);
    return out;
}

std::vector<float> renderKeys (std::size_t frames, float decay, float brightness)
{
    std::vector<float> out (frames, 0.0f);
    for (std::size_t i = 0; i < frames / 10; ++i)
        out[i] = (static_cast<float> (std::rand()) / RAND_MAX) * 2.0f - 1.0f;
    for (std::size_t i = 1; i < frames; ++i)
    {
        const float prev = out[i - 1];
        const float n = out[i > 1 ? i - 2 : frames - 1];
        out[i] = (0.5f * (prev + n)) * decay;
    }
    for (std::size_t i = 0; i < frames; ++i)
        out[i] += brightness * 0.15f * std::sin (2.0f * static_cast<float> (kPi) * static_cast<float> (i) / 48.0f);
    normalizePeak (out, 0.88f);
    return out;
}

std::vector<float> renderPad (std::size_t frames, float cutoff)
{
    std::vector<float> out (frames, 0.0f);
    float lp = 0.0f;
    for (std::size_t i = 0; i < frames; ++i)
    {
        const float white = (static_cast<float> (std::rand()) / RAND_MAX) * 2.0f - 1.0f;
        lp += cutoff * (white - lp);
        out[i] = lp;
    }
    normalizePeak (out, 0.7f);
    return out;
}

std::vector<float> renderSub808 (std::size_t frames, float punch)
{
    std::vector<float> out (frames, 0.0f);
    for (std::size_t i = 0; i < frames; ++i)
    {
        const double t = static_cast<double> (i) / static_cast<double> (frames);
        const double env = std::exp (-3.0 * t) * (1.0 - std::exp (-punch * 40.0 * t));
        const double s = std::sin (2.0 * kPi * t * 2.0) * env;
        out[i] = static_cast<float> (s);
    }
    normalizePeak (out, 0.95f);
    return out;
}

struct RegionSpec final
{
    disklordz::rompler::assets::SampleCategory category;
    std::uint8_t toneIndex;
    std::uint8_t root;
    std::uint8_t lo;
    std::uint8_t hi;
    std::vector<float> pcm;
    std::uint32_t loopStart = 0;
    std::uint32_t loopEnd = 0;
};

void appendRegion (std::vector<RegionSpec>& specs,
                   disklordz::rompler::assets::SampleCategory cat,
                   std::uint8_t tone,
                   int root,
                   int lo,
                   int hi,
                   std::vector<float> pcm,
                   bool looped)
{
    RegionSpec r;
    r.category = cat;
    r.toneIndex = tone;
    r.root = static_cast<std::uint8_t> (root);
    r.lo = static_cast<std::uint8_t> (lo);
    r.hi = static_cast<std::uint8_t> (hi);
    r.pcm = std::move (pcm);
    if (looped && r.pcm.size() > 256)
    {
        r.loopStart = static_cast<std::uint32_t> (r.pcm.size() / 4);
        r.loopEnd = static_cast<std::uint32_t> (r.pcm.size());
    }
    specs.push_back (std::move (r));
}

void buildFactoryRegions (std::vector<RegionSpec>& specs)
{
    const int zoneRoots[] = { 36, 43, 50, 57, 64, 71, 78, 84 };

    for (int i = 0; i < 8; ++i)
    {
        const int root = zoneRoots[i];
        const int lo = (i == 0) ? 0 : zoneRoots[i - 1] + 1;
        const int hi = (i == 7) ? 127 : zoneRoots[i + 1];

        appendRegion (specs, disklordz::rompler::assets::SampleCategory::bell, 0, root, lo, hi,
                      renderBell (4096, 0.4f + 0.05f * static_cast<float> (i), 4.0f + 0.2f * i), false);
        appendRegion (specs, disklordz::rompler::assets::SampleCategory::keys, 1, root, lo, hi,
                      renderKeys (8192, 0.992f - 0.002f * static_cast<float> (i), 0.3f + 0.04f * i), false);
        appendRegion (specs, disklordz::rompler::assets::SampleCategory::pad, 2, root, lo, hi,
                      renderPad (16384, 0.08f + 0.01f * static_cast<float> (i)), true);
        appendRegion (specs, disklordz::rompler::assets::SampleCategory::sub, 3, root, lo, hi,
                      renderSub808 (12000, 0.6f + 0.03f * static_cast<float> (i)), false);
    }
}

bool writePack (const std::string& path, const std::vector<RegionSpec>& specs)
{
    std::vector<float> pool;
    std::vector<disklordz::rompler::assets::SampleRegion> regions;
    regions.reserve (specs.size());

    for (const auto& spec : specs)
    {
        disklordz::rompler::assets::SampleRegion reg{};
        reg.toneIndex = spec.toneIndex;
        reg.rootNote = spec.root;
        reg.loKey = spec.lo;
        reg.hiKey = spec.hi;
        reg.pcmOffset = static_cast<std::uint32_t> (pool.size());
        reg.frameCount = static_cast<std::uint32_t> (spec.pcm.size());
        reg.loopStart = spec.loopStart;
        reg.loopEnd = spec.loopEnd;
        reg.category = static_cast<std::uint16_t> (spec.category);
        pool.insert (pool.end(), spec.pcm.begin(), spec.pcm.end());
        regions.push_back (reg);
    }

    disklordz::rompler::assets::PackHeader hdr{};
    std::memcpy (hdr.magic, disklordz::rompler::assets::kPackMagic, 8);
    hdr.version = disklordz::rompler::assets::kPackVersion;
    hdr.regionCount = static_cast<std::uint32_t> (regions.size());
    hdr.pcmFloatCount = static_cast<std::uint32_t> (pool.size());

    std::ofstream out (path, std::ios::binary);
    if (! out)
        return false;

    out.write (reinterpret_cast<const char*> (&hdr), sizeof (hdr));
    out.write (reinterpret_cast<const char*> (regions.data()),
               static_cast<std::streamsize> (regions.size() * sizeof (regions[0])));
    out.write (reinterpret_cast<const char*> (pool.data()),
               static_cast<std::streamsize> (pool.size() * sizeof (float)));
    return out.good();
}

} // namespace

int main (int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: GenerateDisklordzRomplerPack <output.dlrom>\n";
        return EXIT_FAILURE;
    }

    std::vector<RegionSpec> specs;
    buildFactoryRegions (specs);

    if (! writePack (argv[1], specs))
    {
        std::cerr << "Failed to write pack\n";
        return EXIT_FAILURE;
    }

    std::cout << "Wrote " << specs.size() << " regions to " << argv[1] << '\n';
    return EXIT_SUCCESS;
}
