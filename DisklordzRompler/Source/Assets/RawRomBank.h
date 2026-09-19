#pragma once

#include "disklordz/RawRomFormat.h"

#include <array>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <span>
#include <vector>

namespace disklordz::rompler::assets
{

struct WaveView final
{
    const float* samples = nullptr;
    std::uint32_t frameCount = 0;
    std::uint32_t loopStart = 0;
    std::uint32_t loopEnd = 0;
    std::uint8_t rootNote = 60;
    bool looped = false;
};

class RawRomBank final
{
public:
    bool loadEmbedded (const void* data, std::size_t size) noexcept
    {
        if (data == nullptr || size < sizeof (rawrom::RomHeader))
            return false;
        const auto* bytes = static_cast<const std::uint8_t*> (data);
        return parse (std::span<const std::uint8_t> (bytes, size));
    }

    bool isLoaded() const noexcept { return loaded_; }
    std::size_t getWaveCount() const noexcept { return waves_.size(); }

    /** Tone layer 0…3 → multisample set 1…4 (engine ROM banks). */
    WaveView selectForToneAndNote (std::uint8_t toneIndex, std::uint8_t midiNote) const noexcept
    {
        if (toneIndex >= rawrom::kMaxMultisampleSets)
            return {};

        const auto setId = static_cast<std::uint16_t> (toneIndex + 1);
        return selectForMultisampleSet (setId, midiNote);
    }

    WaveView selectForMultisampleSet (std::uint16_t setId, std::uint8_t midiNote) const noexcept
    {
        if (setId == 0 || setId > multisampleIndex_.size())
            return defaultWave();

        const auto& bucket = multisampleIndex_[static_cast<std::size_t> (setId)];
        if (bucket.empty())
            return defaultWave();

        const float note = static_cast<float> (midiNote);
        std::size_t best = bucket.front();
        float bestDist = std::abs (waves_[best].rootNote - note);

        for (const auto idx : bucket)
        {
            const float d = std::abs (waves_[static_cast<std::size_t> (idx)].rootNote - note);
            if (d < bestDist)
            {
                bestDist = d;
                best = idx;
            }
        }
        return waves_[best];
    }

private:
    WaveView defaultWave() const noexcept
    {
        if (waves_.empty())
            return {};
        return waves_.front();
    }

    bool parse (std::span<const std::uint8_t> bytes) noexcept
    {
        loaded_ = false;
        waves_.clear();
        pool_.clear();
        for (auto& b : multisampleIndex_)
            b.clear();

        rawrom::RomHeader hdr{};
        std::memcpy (&hdr, bytes.data(), sizeof (hdr));
        if (std::memcmp (hdr.magic, rawrom::kRomMagic, 8) != 0 || hdr.version != rawrom::kRomVersion)
            return false;

        const std::size_t tableBytes = static_cast<std::size_t> (hdr.waveCount) * sizeof (rawrom::WaveEntry);
        const std::size_t pcmBytes = static_cast<std::size_t> (hdr.pcmFloatCount) * sizeof (float);
        if (bytes.size() < sizeof (hdr) + tableBytes + pcmBytes)
            return false;

        const auto* table = reinterpret_cast<const rawrom::WaveEntry*> (bytes.data() + sizeof (hdr));
        const auto* pcm = reinterpret_cast<const float*> (bytes.data() + sizeof (hdr) + tableBytes);
        pool_.assign (pcm, pcm + hdr.pcmFloatCount);

        waves_.resize (hdr.waveCount);
        for (std::uint32_t i = 0; i < hdr.waveCount; ++i)
        {
            const auto& e = table[i];
            if (e.pcmOffset + e.numFrames > hdr.pcmFloatCount)
                continue;

            WaveView v;
            v.samples = pool_.data() + e.pcmOffset;
            v.frameCount = e.numFrames;
            v.loopStart = e.loopStart;
            v.loopEnd = e.loopEnd;
            v.rootNote = static_cast<std::uint8_t> (e.rootMidiNote);
            v.looped = (e.flags & rawrom::waveLooped) != 0;
            waves_[i] = v;

            if (e.multisampleSetId > 0 && e.multisampleSetId < multisampleIndex_.size())
                multisampleIndex_[static_cast<std::size_t> (e.multisampleSetId)].push_back (i);
        }

        loaded_ = ! waves_.empty();
        return loaded_;
    }

    std::vector<float> pool_;
    std::vector<WaveView> waves_;
    std::array<std::vector<std::uint32_t>, rawrom::kMaxMultisampleSets + 1> multisampleIndex_{};
    bool loaded_ = false;
};

} // namespace disklordz::rompler::assets
