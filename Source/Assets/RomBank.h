#pragma once

#include "RomFormat.h"
#include "DSP/PcmWaveform.h"

#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace jdupgraded::assets
{

struct WaveSelection final
{
    const dsp::PcmWaveform* wave = nullptr;
    float rootMidiNote = 60.0f;
    std::size_t waveIndex = 0;
};

class RomBank final
{
public:
    RomBank() = default;

    bool loadEmbedded (const void* data, std::size_t size) noexcept;
    bool loadFromFile (const std::string& path);

    bool isLoaded() const noexcept { return loaded_; }
    std::size_t getWaveCount() const noexcept { return waveforms_.size(); }

    const dsp::PcmWaveform& getWave (std::size_t index) const noexcept;
    float getRootMidiNote (std::size_t index) const noexcept;
    std::uint16_t getMultisampleSetId (std::size_t index) const noexcept;
    RomWaveCategory getWaveCategory (std::size_t index) const noexcept;

    /** First standalone (non-multisample) wave in category, or first wave in category if none. */
    std::size_t findFirstWaveInCategory (RomWaveCategory category) const noexcept;

    WaveSelection selectForNote (std::uint16_t multisampleSetId, std::uint8_t midiNote) const noexcept;
    WaveSelection selectFixedWave (std::size_t waveIndex) const noexcept;

    const dsp::PcmWaveform& getDefaultWave() const noexcept;

    const char* getLoadError() const noexcept { return loadError_; }

private:
    bool parseRom (std::span<const std::uint8_t> bytes) noexcept;
    void buildMultisampleIndex() noexcept;

    struct WaveMeta final
    {
        float rootMidiNote = 60.0f;
        std::uint16_t multisampleSetId = 0;
        RomWaveCategory category = RomWaveCategory::analog;
    };

    std::vector<float> pcmPool_;
    std::vector<dsp::PcmWaveform> waveforms_;
    std::vector<WaveMeta> meta_;
    std::array<std::vector<std::size_t>, kMultisampleSetCount + 1> multisampleIndex_{};
    dsp::PcmWaveform nullWave_{};
    bool loaded_ = false;
    const char* loadError_ = nullptr;
};

} // namespace jdupgraded::assets
