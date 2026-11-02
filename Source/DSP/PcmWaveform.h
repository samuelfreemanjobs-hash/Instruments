#pragma once

#include <cstddef>
#include <cstdint>

namespace jdupgraded::dsp
{

enum class WaveformMode : std::uint8_t
{
    singleCycle,
    loopedPcm
};

struct PcmWaveform final
{
    const float* samples = nullptr;
    std::size_t numFrames = 0;
    std::size_t loopStart = 0;
    std::size_t loopEnd = 0;
    WaveformMode mode = WaveformMode::singleCycle;

    bool isValid() const noexcept
    {
        return samples != nullptr && numFrames > 0;
    }

    std::size_t effectiveLoopEnd() const noexcept
    {
        if (mode == WaveformMode::singleCycle)
            return numFrames;

        if (loopEnd > loopStart && loopEnd <= numFrames)
            return loopEnd;

        return numFrames;
    }

    std::size_t loopLength() const noexcept
    {
        const auto end = effectiveLoopEnd();
        return end > loopStart ? end - loopStart : numFrames;
    }
};

} // namespace jdupgraded::dsp
