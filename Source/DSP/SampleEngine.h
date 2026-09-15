#pragma once

#include "PcmWaveform.h"

#include <algorithm>
#include <cmath>

namespace jdupgraded::dsp
{

/** Band-limited-ready PCM oscillator with linear interpolation (sync/mod hooks in phase 2). */
class SampleEngine final
{
public:
    void reset() noexcept
    {
        phase_ = 0.0;
        pitchRatio_ = 1.0f;
        waveform_ = {};
    }

    void setWaveform (const PcmWaveform& waveform) noexcept
    {
        waveform_ = waveform;
        if (waveform_.isValid())
            phase_ = std::fmod (phase_, static_cast<double> (waveform_.numFrames));
    }

    void setPitchRatio (float ratio) noexcept
    {
        pitchRatio_ = std::max (0.0001f, ratio);
    }

    float renderSample() noexcept
    {
        if (! waveform_.isValid())
            return 0.0f;

        const auto idx0 = static_cast<std::size_t> (phase_);
        const auto idx1 = (idx0 + 1) % waveform_.numFrames;
        const float frac = static_cast<float> (phase_ - static_cast<double> (idx0));

        const float s0 = waveform_.samples[idx0];
        const float s1 = waveform_.samples[idx1];
        const float out = s0 + frac * (s1 - s0);

        advancePhase();
        return out;
    }

    void renderBlock (float* dest, std::size_t numSamples, float pitchMod) noexcept
    {
        const float ratio = pitchRatio_ * pitchMod;
        for (std::size_t i = 0; i < numSamples; ++i)
        {
            setPitchRatio (ratio);
            dest[i] = renderSample();
        }
    }

    double getPhase() const noexcept { return phase_; }
    void setPhase (double phase) noexcept { phase_ = phase; }

private:
    void advancePhase() noexcept
    {
        phase_ += static_cast<double> (pitchRatio_);

        if (waveform_.mode == WaveformMode::singleCycle)
        {
            const double len = static_cast<double> (waveform_.numFrames);
            while (phase_ >= len)
                phase_ -= len;
            while (phase_ < 0.0)
                phase_ += len;
            return;
        }

        const auto loopStart = static_cast<double> (waveform_.loopStart);
        const auto loopEnd = static_cast<double> (waveform_.effectiveLoopEnd());
        const auto loopLen = loopEnd - loopStart;

        if (phase_ >= loopEnd && loopLen > 1.0)
        {
            phase_ = loopStart + std::fmod (phase_ - loopStart, loopLen);
        }
        else if (phase_ >= static_cast<double> (waveform_.numFrames))
        {
            phase_ = loopStart;
        }
    }

    PcmWaveform waveform_{};
    double phase_ = 0.0;
    float pitchRatio_ = 1.0f;
};

} // namespace jdupgraded::dsp
