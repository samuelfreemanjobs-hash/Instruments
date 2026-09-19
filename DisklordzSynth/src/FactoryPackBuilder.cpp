#include "disklordz/FactoryPackBuilder.h"
#include "disklordz/ProceduralSynth.h"

#include <cstring>
#include <fstream>

namespace disklordz::synth
{

namespace
{

void appendRegion (std::vector<PackRegionSpec>& specs,
                   dlrom::SampleCategory cat,
                   std::uint8_t tone,
                   int root,
                   int lo,
                   int hi,
                   std::vector<float> pcm,
                   bool looped)
{
    PackRegionSpec r;
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

} // namespace

void buildRomplerFactoryRegions (std::vector<PackRegionSpec>& specs)
{
    const int zoneRoots[] = { 36, 43, 50, 57, 64, 71, 78, 84 };

    for (int i = 0; i < 8; ++i)
    {
        const int root = zoneRoots[i];
        const int lo = (i == 0) ? 0 : zoneRoots[i - 1] + 1;
        const int hi = (i == 7) ? 127 : zoneRoots[i + 1];

        appendRegion (specs, dlrom::SampleCategory::bell, 0, root, lo, hi,
                      renderBell (4096, 0.4f + 0.05f * static_cast<float> (i), 4.0f + 0.2f * i), false);
        appendRegion (specs, dlrom::SampleCategory::keys, 1, root, lo, hi,
                      renderKeys (8192, 0.992f - 0.002f * static_cast<float> (i), 0.3f + 0.04f * i), false);
        appendRegion (specs, dlrom::SampleCategory::pad, 2, root, lo, hi,
                      renderPad (16384, 0.08f + 0.01f * static_cast<float> (i), false), true);
        appendRegion (specs, dlrom::SampleCategory::sub, 3, root, lo, hi,
                      renderSub808 (12000, 0.6f + 0.03f * static_cast<float> (i)), false);
    }
}

bool writeDlromPack (const std::string& path, const std::vector<PackRegionSpec>& specs)
{
    std::vector<float> pool;
    std::vector<dlrom::SampleRegion> regions;
    regions.reserve (specs.size());

    for (const auto& spec : specs)
    {
        dlrom::SampleRegion reg{};
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

    dlrom::PackHeader hdr{};
    std::memcpy (hdr.magic, dlrom::kPackMagic, 8);
    hdr.version = dlrom::kPackVersion;
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

} // namespace disklordz::synth
