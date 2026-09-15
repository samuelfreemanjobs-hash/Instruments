#pragma once

#include "RomFormat.h"
#include "DSP/PcmWaveform.h"

#include <span>
#include <string>
#include <vector>

namespace jdupgraded::assets
{

class RomBank final
{
public:
    RomBank() = default;

    bool loadEmbedded (const void* data, std::size_t size) noexcept;
    bool loadFromFile (const std::string& path);

    bool isLoaded() const noexcept { return loaded_; }
    std::size_t getWaveCount() const noexcept { return waveforms_.size(); }

    const dsp::PcmWaveform& getWave (std::size_t index) const noexcept;
    const dsp::PcmWaveform& getDefaultWave() const noexcept;

    const char* getLoadError() const noexcept { return loadError_; }

private:
    bool parseRom (std::span<const std::uint8_t> bytes) noexcept;

    std::vector<float> pcmPool_;
    std::vector<dsp::PcmWaveform> waveforms_;
    dsp::PcmWaveform nullWave_{};
    bool loaded_ = false;
    const char* loadError_ = nullptr;
};

} // namespace jdupgraded::assets
