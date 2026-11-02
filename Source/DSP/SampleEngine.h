#pragma once

#include "Modulation/PolyBlep.h"
#include "PcmWaveform.h"

#include <algorithm>
#include <cmath>

namespace jdupgraded::dsp
{

/** PCM oscillator with linear interpolation, PolyBLEP hard sync, and phase cross-mod hooks. */
class SampleEngine final
{
public:
    void reset() noexcept
    {
        phase_ = 0.0;
        lastMasterPhase_ = 0.0;
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

    void setHardSyncFrom (const SampleEngine* master) noexcept
    {
        syncMaster_ = master;
    }

    void setPhaseModDepth (float depth) noexcept
    {
        phaseModDepth_ = std::clamp (depth, 0.0f, 2.0f);
    }

    void setPhaseModSource (float sourceSample) noexcept
    {
        phaseModSource_ = sourceSample;
    }

    float renderSample() noexcept
    {
        if (! waveform_.isValid())
            return 0.0f;

        const double cycleLen = static_cast<double> (waveform_.numFrames);
        float blepResidual = 0.0f;

        if (syncMaster_ != nullptr)
        {
            const double masterPhase = syncMaster_->phase_;
            if (masterPhase < lastMasterPhase_)
            {
                const double dt = static_cast<double> (pitchRatio_) / cycleLen;
                const double t = phase_ / cycleLen;
                blepResidual = polyBlep (t, dt);
                phase_ = 0.0;
            }
            lastMasterPhase_ = masterPhase;
        }

        const double modulatedPhase = phase_ + static_cast<double> (phaseModSource_ * phaseModDepth_ * cycleLen * 0.08);
        const double wrappedPhase = wrapPhase (modulatedPhase, cycleLen);

        const auto idx0 = static_cast<std::size_t> (wrappedPhase) % waveform_.numFrames;
        const auto idx1 = (idx0 + 1) % waveform_.numFrames;
        const float frac = static_cast<float> (wrappedPhase - std::floor (wrappedPhase));

        const float s0 = waveform_.samples[idx0];
        const float s1 = waveform_.samples[idx1];
        float out = s0 + frac * (s1 - s0);
        out -= blepResidual;

        advancePhase();
        return out;
    }

    double getPhase() const noexcept { return phase_; }
    void setPhase (double phase) noexcept { phase_ = phase; }

private:
    static double wrapPhase (double phase, double cycleLen) noexcept
    {
        double p = phase;
        while (p >= cycleLen)
            p -= cycleLen;
        while (p < 0.0)
            p += cycleLen;
        return p;
    }

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
            phase_ = loopStart + std::fmod (phase_ - loopStart, loopLen);
        else if (phase_ >= static_cast<double> (waveform_.numFrames))
            phase_ = loopStart;
    }

    PcmWaveform waveform_{};
    const SampleEngine* syncMaster_ = nullptr;
    double phase_ = 0.0;
    double lastMasterPhase_ = 0.0;
    float pitchRatio_ = 1.0f;
    float phaseModDepth_ = 0.0f;
    float phaseModSource_ = 0.0f;
};

} // namespace jdupgraded::dsp
