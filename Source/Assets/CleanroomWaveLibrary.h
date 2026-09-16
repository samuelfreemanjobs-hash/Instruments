#pragma once

#include "DSP/PcmWaveform.h"

#include <array>
#include <cmath>
#include <cstddef>

namespace jdupgraded::assets
{

/** Phase 3: optional cleanroom single-cycle waves (no Roland ROM data). */
class CleanroomWaveLibrary final
{
public:
    static constexpr std::size_t kNumWaves = 4;
    static constexpr std::size_t kWaveLength = 2048;

    CleanroomWaveLibrary() noexcept { buildWaves(); }

    const dsp::PcmWaveform& getWave (std::size_t index) const noexcept
    {
        return waveforms_[index % kNumWaves];
    }

private:
    void buildWaves() noexcept
    {
        for (std::size_t w = 0; w < kNumWaves; ++w)
        {
            for (std::size_t i = 0; i < kWaveLength; ++i)
            {
                const float phase = static_cast<float> (i) / static_cast<float> (kWaveLength);
                const float harmonic = static_cast<float> (w + 1);
                buffers_[w][i] = std::sin (2.0f * 3.14159265f * phase * harmonic) / harmonic;
            }

            waveforms_[w].samples = buffers_[w].data();
            waveforms_[w].numFrames = kWaveLength;
            waveforms_[w].loopStart = 0;
            waveforms_[w].loopEnd = kWaveLength;
            waveforms_[w].mode = dsp::WaveformMode::singleCycle;
        }
    }

    std::array<std::array<float, kWaveLength>, kNumWaves> buffers_{};
    std::array<dsp::PcmWaveform, kNumWaves> waveforms_{};
};

} // namespace jdupgraded::assets
