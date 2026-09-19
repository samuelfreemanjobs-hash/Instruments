#pragma once

#include "disklordz/RawRomBundleFormat.h"
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
        if (data == nullptr || size < 8)
            return false;
        const auto* bytes = static_cast<const std::uint8_t*> (data);
        return parse (std::span<const std::uint8_t> (bytes, size));
    }

    bool isLoaded() const noexcept { return loaded_; }
    std::size_t getWaveCount() const noexcept { return waves_.size(); }
    std::uint32_t getBankIndex() const noexcept { return bankIndex_; }

    WaveView selectForMultisampleSet (std::uint16_t setId, std::uint8_t midiNote) const noexcept
    {
        if (setId == 0 || setId >= multisampleIndex_.size())
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

    /** Tone layer 0…3 with default program 0 on this card. */
    WaveView selectForToneAndNote (std::uint8_t toneIndex, std::uint8_t midiNote) const noexcept
    {
        if (toneIndex >= rawrom::kRomplerToneLayers)
            return {};
        const auto setId = rawrom::multisampleSetForToneProgram (toneIndex, 0);
        return selectForMultisampleSet (setId, midiNote);
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
        bankIndex_ = 0;
        for (auto& b : multisampleIndex_)
            b.clear();

        if (bytes.size() < 8)
            return false;

        char magic[8];
        std::memcpy (magic, bytes.data(), 8);
        if (std::memcmp (magic, rawrom::kRomMagic, 8) != 0)
            return false;

        std::uint32_t version = 0;
        std::memcpy (&version, bytes.data() + 8, 4);

        std::uint32_t waveCount = 0;
        std::uint32_t pcmFloatCount = 0;
        std::size_t headerSize = 0;

        if (version == rawrom::kRomVersion1)
        {
            if (bytes.size() < 20)
                return false;
            std::memcpy (&waveCount, bytes.data() + 12, 4);
            std::memcpy (&pcmFloatCount, bytes.data() + 16, 4);
            headerSize = 20;
            bankIndex_ = 0;
        }
        else if (version == rawrom::kRomVersion)
        {
            if (bytes.size() < sizeof (rawrom::RomHeader))
                return false;
            rawrom::RomHeader hdr{};
            std::memcpy (&hdr, bytes.data(), sizeof (hdr));
            waveCount = hdr.waveCount;
            pcmFloatCount = hdr.pcmFloatCount;
            bankIndex_ = hdr.bankIndex;
            headerSize = sizeof (hdr);
        }
        else
        {
            return false;
        }

        const std::size_t tableBytes = static_cast<std::size_t> (waveCount) * sizeof (rawrom::WaveEntry);
        const std::size_t pcmBytes = static_cast<std::size_t> (pcmFloatCount) * sizeof (float);
        if (bytes.size() < headerSize + tableBytes + pcmBytes)
            return false;

        const auto* table = reinterpret_cast<const rawrom::WaveEntry*> (bytes.data() + headerSize);
        const auto* pcm = reinterpret_cast<const float*> (bytes.data() + headerSize + tableBytes);
        pool_.assign (pcm, pcm + pcmFloatCount);

        waves_.resize (waveCount);
        for (std::uint32_t i = 0; i < waveCount; ++i)
        {
            const auto& e = table[i];
            if (e.pcmOffset + e.numFrames > pcmFloatCount)
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
    std::array<std::vector<std::uint32_t>, rawrom::kMaxMultisampleSetsPerRom + 1> multisampleIndex_{};
    std::uint32_t bankIndex_ = 0;
    bool loaded_ = false;
};

struct RomBlob final
{
    const void* data = nullptr;
    std::size_t size = 0;
};

/** Multi-card PCM library (all embedded `.dlrrom` expansion ROMs). */
class RawRomLibrary final
{
public:
    bool loadBankEmbedded (const void* data, std::size_t size) noexcept
    {
        RawRomBank bank;
        if (! bank.loadEmbedded (data, size))
            return false;
        banks_.push_back (std::move (bank));
        return true;
    }

    bool loadFactoryEmbedded (std::span<const RomBlob> blobs) noexcept
    {
        banks_.clear();
        bool any = false;
        for (const auto& blob : blobs)
        {
            if (blob.data == nullptr || blob.size == 0)
                continue;
            if (loadBankEmbedded (blob.data, blob.size))
                any = true;
        }
        sortBanks();
        return any && ! banks_.empty();
    }

    /** Load all `.dlrrom` payloads from a DLRROMCAT factory bundle (single embed). */
    bool loadFactoryBundle (const void* data, std::size_t size) noexcept
    {
        banks_.clear();
        if (data == nullptr || size < sizeof (rawrom::RomBundleHeader))
            return false;

        const auto* bytes = static_cast<const std::uint8_t*> (data);
        rawrom::RomBundleHeader hdr{};
        std::memcpy (&hdr, bytes, sizeof (hdr));
        if (std::memcmp (hdr.magic, rawrom::kRomBundleMagic, 8) != 0
            || hdr.version != rawrom::kRomBundleVersion)
            return false;

        const std::size_t tableBytes = static_cast<std::size_t> (hdr.fileCount) * sizeof (rawrom::RomBundleEntry);
        if (size < sizeof (hdr) + tableBytes)
            return false;

        const auto* table = reinterpret_cast<const rawrom::RomBundleEntry*> (bytes + sizeof (hdr));
        for (std::uint32_t i = 0; i < hdr.fileCount; ++i)
        {
            const auto& ent = table[i];
            if (ent.offset + ent.size > size)
                continue;
            loadBankEmbedded (bytes + ent.offset, ent.size);
        }
        sortBanks();
        return ! banks_.empty();
    }

    std::size_t getBankCount() const noexcept { return banks_.size(); }

    std::size_t getTotalWaveCount() const noexcept
    {
        std::size_t n = 0;
        for (const auto& b : banks_)
            n += b.getWaveCount();
        return n;
    }

    WaveView select (std::uint8_t bankIndex, std::uint16_t multisampleSetId, std::uint8_t midiNote) const noexcept
    {
        const RawRomBank* bank = findBank (bankIndex);
        if (bank == nullptr && ! banks_.empty())
            bank = &banks_.front();
        if (bank == nullptr)
            return {};
        return bank->selectForMultisampleSet (multisampleSetId, midiNote);
    }

    WaveView selectForTone (std::uint8_t romBankIndex,
                            std::uint8_t toneIndex,
                            std::uint8_t programVariant,
                            std::uint8_t midiNote) const noexcept
    {
        const auto setId = rawrom::multisampleSetForToneProgram (toneIndex, programVariant);
        return select (romBankIndex, setId, midiNote);
    }

private:
    void sortBanks() noexcept
    {
        std::sort (banks_.begin(), banks_.end(), [] (const RawRomBank& a, const RawRomBank& b) {
            return a.getBankIndex() < b.getBankIndex();
        });
    }

    const RawRomBank* findBank (std::uint8_t bankIndex) const noexcept
    {
        for (const auto& b : banks_)
            if (b.getBankIndex() == static_cast<std::uint32_t> (bankIndex))
                return &b;
        if (bankIndex < banks_.size())
            return &banks_[bankIndex];
        return nullptr;
    }

    std::vector<RawRomBank> banks_;
};

} // namespace disklordz::rompler::assets
