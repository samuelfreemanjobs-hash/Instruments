#include "RomBank.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>

namespace jdupgraded::assets
{

bool RomBank::loadEmbedded (const void* data, std::size_t size) noexcept
{
    if (data == nullptr || size < sizeof (RomHeader))
    {
        loadError_ = "Embedded ROM missing";
        return false;
    }

    const auto* bytes = static_cast<const std::uint8_t*> (data);
    return parseRom ({ bytes, size });
}

bool RomBank::loadFromFile (const std::string& path)
{
    std::ifstream in (path, std::ios::binary | std::ios::ate);
    if (! in)
    {
        loadError_ = "ROM file not found";
        return false;
    }

    const auto size = static_cast<std::size_t> (in.tellg());
    in.seekg (0);
    std::vector<std::uint8_t> buffer (size);
    in.read (reinterpret_cast<char*> (buffer.data()), static_cast<std::streamsize> (size));
    return parseRom (buffer);
}

bool RomBank::parseRom (std::span<const std::uint8_t> bytes) noexcept
{
    loaded_ = false;
    waveforms_.clear();
    pcmPool_.clear();
    meta_.clear();
    for (auto& bucket : multisampleIndex_)
        bucket.clear();
    loadError_ = nullptr;

    if (bytes.size() < sizeof (RomHeader))
    {
        loadError_ = "ROM too small";
        return false;
    }

    RomHeader header{};
    std::memcpy (&header, bytes.data(), sizeof (header));
    if (std::memcmp (header.magic, kRomMagic, sizeof (kRomMagic)) != 0)
    {
        loadError_ = "Invalid ROM magic";
        return false;
    }

    if (header.version < 1 || header.version > kRomVersion || header.waveCount == 0)
    {
        loadError_ = "Unsupported ROM version";
        return false;
    }

    const std::size_t tableBytes = static_cast<std::size_t> (header.waveCount) * sizeof (RomWaveEntry);
    const std::size_t tableStart = header.waveTableOffset;
    const std::size_t pcmStart = header.pcmDataOffset;

    if (tableStart + tableBytes > bytes.size() || pcmStart + header.pcmBytes > bytes.size())
    {
        loadError_ = "ROM layout corrupt";
        return false;
    }

    const auto crc = crc32 (bytes.data() + pcmStart, header.pcmBytes);
    if (crc != header.crc32)
    {
        loadError_ = "ROM checksum mismatch";
        return false;
    }

    pcmPool_.resize (header.pcmBytes / sizeof (std::int16_t));
    const auto* pcm16 = reinterpret_cast<const std::int16_t*> (bytes.data() + pcmStart);
    for (std::size_t i = 0; i < pcmPool_.size(); ++i)
        pcmPool_[i] = static_cast<float> (pcm16[i]) * (1.0f / 32768.0f);

    waveforms_.resize (header.waveCount);
    meta_.resize (header.waveCount);

    for (std::uint32_t i = 0; i < header.waveCount; ++i)
    {
        RomWaveEntry entry{};
        const std::size_t entryOffset = tableStart + static_cast<std::size_t> (i) * sizeof (RomWaveEntry);
        std::memcpy (&entry, bytes.data() + entryOffset, sizeof (entry));

        const std::size_t sampleOffset = entry.pcmOffsetBytes / sizeof (std::int16_t);
        if (sampleOffset + entry.numFrames > pcmPool_.size())
        {
            loadError_ = "Wave PCM out of range";
            waveforms_.clear();
            return false;
        }

        auto& wave = waveforms_[i];
        wave.samples = pcmPool_.data() + sampleOffset;
        wave.numFrames = entry.numFrames;
        wave.loopStart = entry.loopStart;
        wave.loopEnd = entry.loopEnd;
        wave.mode = (entry.flags & romWaveLooped) != 0 ? dsp::WaveformMode::loopedPcm
                                                       : dsp::WaveformMode::singleCycle;

        meta_[i].rootMidiNote = static_cast<float> (entry.rootMidiNote);
        if (header.version >= 2)
            meta_[i].multisampleSetId = entry.multisampleSetId;
        else
            meta_[i].multisampleSetId = 0;

        meta_[i].category = static_cast<RomWaveCategory> (entry.category % 9);
    }

    buildMultisampleIndex();
    loaded_ = true;
    return true;
}

void RomBank::buildMultisampleIndex() noexcept
{
    for (std::size_t i = 0; i < waveforms_.size(); ++i)
    {
        const auto setId = meta_[i].multisampleSetId;
        if (setId > 0 && setId <= kMultisampleSetCount)
            multisampleIndex_[setId].push_back (i);
    }

    for (std::uint16_t setId = 1; setId <= kMultisampleSetCount; ++setId)
    {
        auto& bucket = multisampleIndex_[setId];
        std::sort (bucket.begin(), bucket.end(), [this] (std::size_t a, std::size_t b)
        {
            return meta_[a].rootMidiNote < meta_[b].rootMidiNote;
        });
    }
}

WaveSelection RomBank::selectForNote (std::uint16_t multisampleSetId, std::uint8_t midiNote) const noexcept
{
    if (! loaded_ || multisampleSetId == 0 || multisampleSetId > kMultisampleSetCount)
        return selectFixedWave (0);

    const auto& bucket = multisampleIndex_[multisampleSetId];
    if (bucket.empty())
        return selectFixedWave (0);

    const float note = static_cast<float> (midiNote);
    std::size_t best = bucket.front();
    float bestDistance = std::abs (meta_[best].rootMidiNote - note);

    for (const auto index : bucket)
    {
        const float distance = std::abs (meta_[index].rootMidiNote - note);
        if (distance < bestDistance)
        {
            bestDistance = distance;
            best = index;
        }
    }

    WaveSelection selection;
    selection.wave = &waveforms_[best];
    selection.rootMidiNote = meta_[best].rootMidiNote;
    selection.waveIndex = best;
    return selection;
}

WaveSelection RomBank::selectFixedWave (std::size_t waveIndex) const noexcept
{
    WaveSelection selection;
    if (! loaded_ || waveforms_.empty())
        return selection;

    const auto index = waveIndex % waveforms_.size();
    selection.wave = &waveforms_[index];
    selection.rootMidiNote = meta_[index].rootMidiNote;
    selection.waveIndex = index;
    return selection;
}

const dsp::PcmWaveform& RomBank::getWave (std::size_t index) const noexcept
{
    if (! loaded_ || waveforms_.empty())
        return nullWave_;

    return waveforms_[index % waveforms_.size()];
}

float RomBank::getRootMidiNote (std::size_t index) const noexcept
{
    if (! loaded_ || meta_.empty())
        return 60.0f;

    return meta_[index % meta_.size()].rootMidiNote;
}

std::uint16_t RomBank::getMultisampleSetId (std::size_t index) const noexcept
{
    if (! loaded_ || meta_.empty())
        return 0;

    return meta_[index % meta_.size()].multisampleSetId;
}

RomWaveCategory RomBank::getWaveCategory (std::size_t index) const noexcept
{
    if (! loaded_ || meta_.empty())
        return RomWaveCategory::analog;

    return meta_[index % meta_.size()].category;
}

std::size_t RomBank::findFirstWaveInCategory (RomWaveCategory category) const noexcept
{
    if (! loaded_ || meta_.empty())
        return 0;

    std::size_t fallback = 0;
    for (std::size_t i = 0; i < meta_.size(); ++i)
    {
        if (meta_[i].category != category)
            continue;

        fallback = i;
        if (meta_[i].multisampleSetId == 0)
            return i;
    }

    return fallback;
}

const dsp::PcmWaveform& RomBank::getDefaultWave() const noexcept
{
    return getWave (0);
}

} // namespace jdupgraded::assets
