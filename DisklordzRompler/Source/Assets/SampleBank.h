#pragma once

#include "Assets/PackFormat.h"

#include <cstring>
#include <span>
#include <vector>

namespace disklordz::rompler::assets
{

struct SampleView final
{
    const float* samples = nullptr;
    std::uint32_t frameCount = 0;
    std::uint32_t loopStart = 0;
    std::uint32_t loopEnd = 0;
    std::uint8_t rootNote = 60;
    std::uint8_t loKey = 0;
    std::uint8_t hiKey = 127;
    std::uint8_t toneIndex = 0;
};

class SampleBank final
{
public:
    bool loadEmbedded (const void* data, std::size_t size) noexcept
    {
        if (data == nullptr || size < sizeof (PackHeader))
            return false;

        const auto* bytes = static_cast<const std::uint8_t*> (data);
        return parse (std::span<const std::uint8_t> (bytes, size));
    }

    bool isLoaded() const noexcept { return loaded_; }

    const SampleView* findRegion (std::uint8_t toneIndex, std::uint8_t midiNote) const noexcept
    {
        for (const auto& r : regions_)
        {
            if (r.toneIndex == toneIndex && midiNote >= r.loKey && midiNote <= r.hiKey)
                return &r;
        }
        return nullptr;
    }

    std::size_t getRegionCount() const noexcept { return regions_.size(); }

private:
    bool parse (std::span<const std::uint8_t> bytes) noexcept
    {
        loaded_ = false;
        regions_.clear();
        pool_.clear();

        if (bytes.size() < sizeof (PackHeader))
            return false;

        PackHeader hdr{};
        std::memcpy (&hdr, bytes.data(), sizeof (hdr));
        if (std::memcmp (hdr.magic, kPackMagic, 8) != 0 || hdr.version != kPackVersion)
            return false;

        const std::size_t tableBytes = static_cast<std::size_t> (hdr.regionCount) * sizeof (SampleRegion);
        const std::size_t pcmBytes = static_cast<std::size_t> (hdr.pcmFloatCount) * sizeof (float);
        const std::size_t expected = sizeof (PackHeader) + tableBytes + pcmBytes;
        if (bytes.size() < expected)
            return false;

        const auto* regionTable = reinterpret_cast<const SampleRegion*> (bytes.data() + sizeof (PackHeader));
        const auto* pcm = reinterpret_cast<const float*> (bytes.data() + sizeof (PackHeader) + tableBytes);

        pool_.assign (pcm, pcm + hdr.pcmFloatCount);

        for (std::uint32_t i = 0; i < hdr.regionCount; ++i)
        {
            const auto& reg = regionTable[i];
            if (reg.pcmOffset + reg.frameCount > hdr.pcmFloatCount)
                continue;

            SampleView view;
            view.samples = pool_.data() + reg.pcmOffset;
            view.frameCount = reg.frameCount;
            view.loopStart = reg.loopStart;
            view.loopEnd = reg.loopEnd;
            view.rootNote = reg.rootNote;
            view.loKey = reg.loKey;
            view.hiKey = reg.hiKey;
            view.toneIndex = reg.toneIndex;
            regions_.push_back (view);
        }

        loaded_ = ! regions_.empty();
        return loaded_;
    }

    std::vector<float> pool_;
    std::vector<SampleView> regions_;
    bool loaded_ = false;
};

} // namespace disklordz::rompler::assets
