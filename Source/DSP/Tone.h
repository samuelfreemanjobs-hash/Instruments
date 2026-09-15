#pragma once

#include "AudioConstants.h"
#include "RateLevelEnvelope.h"
#include "SampleEngine.h"

#include <cmath>

namespace jdupgraded::dsp
{

struct TonePatch final
{
    const PcmWaveform* waveform = nullptr;
    float coarseSemis = 0.0f;
    float fineCents = 0.0f;
    float level = 1.0f;
    float attackTimeSec = 0.002f;
    float decayTimeSec = 0.15f;
    float sustainLevel = 0.85f;
    float releaseTimeSec = 0.25f;
    float filterCutoffNorm = 1.0f;
    float filterAttackTimeSec = 0.005f;
    float filterDecayTimeSec = 0.2f;
    float filterSustainLevel = 0.7f;
    float filterReleaseTimeSec = 0.3f;
};

class Tone final
{
public:
    void setSampleRate (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate;
        pitchEnvelope_.setSampleRate (sampleRate);
        cutoffEnvelope_.setSampleRate (sampleRate);
        ampEnvelope_.setSampleRate (sampleRate);
    }

    void applyPatch (const TonePatch& patch) noexcept
    {
        patch_ = patch;
        if (patch.waveform != nullptr)
            engine_.setWaveform (*patch.waveform);

        const float rootRatio = semitoneRatio (patch.coarseSemis + patch.fineCents / 100.0f);
        basePitchRatio_ = rootRatio;
        engine_.setPitchRatio (basePitchRatio_);
    }

    void start (std::uint8_t midiNote) noexcept
    {
        midiNote_ = midiNote;
        const float noteRatio = semitoneRatio (static_cast<float> (midiNote) - 60.0f);
        engine_.reset();
        engine_.setPitchRatio (basePitchRatio_ * noteRatio);

        pitchEnvelope_.reset (0.0f);
        pitchEnvelope_.noteOn (1.0f, patch_.attackTimeSec, 1.0f, patch_.decayTimeSec, 1.0f);
        pitchEnvelope_.configureRelease (patch_.releaseTimeSec);

        cutoffEnvelope_.reset (0.0f);
        cutoffEnvelope_.noteOn (patch_.filterCutoffNorm, patch_.filterAttackTimeSec,
                                patch_.filterCutoffNorm * 0.9f, patch_.filterDecayTimeSec,
                                patch_.filterSustainLevel);
        cutoffEnvelope_.configureRelease (patch_.filterReleaseTimeSec);

        ampEnvelope_.reset (0.0f);
        ampEnvelope_.noteOn (1.0f, patch_.attackTimeSec, 0.9f, patch_.decayTimeSec, patch_.sustainLevel);
        ampEnvelope_.configureRelease (patch_.releaseTimeSec);

        active_ = true;
        samplesUntilControlTick_ = 0;
    }

    void release() noexcept
    {
        pitchEnvelope_.noteOff (patch_.releaseTimeSec);
        cutoffEnvelope_.noteOff (patch_.filterReleaseTimeSec);
        ampEnvelope_.noteOff (patch_.releaseTimeSec);
    }

    bool isActive() const noexcept
    {
        return active_ && (ampEnvelope_.isActive() || pitchEnvelope_.isActive());
    }

    void render (float* output, std::size_t numSamples) noexcept
    {
        if (! active_)
        {
            for (std::size_t i = 0; i < numSamples; ++i)
                output[i] = 0.0f;
            return;
        }

        for (std::size_t i = 0; i < numSamples; ++i)
        {
            if (samplesUntilControlTick_ == 0)
            {
                pitchEnvelope_.advanceControlTick();
                cutoffEnvelope_.advanceControlTick();
                ampEnvelope_.advanceControlTick();
                samplesUntilControlTick_ = kControlRateDivisor;
            }
            --samplesUntilControlTick_;

            const float pitchEnv = pitchEnvelope_.getLevel();
            const float cutoffEnv = cutoffEnvelope_.getLevel();
            const float ampEnv = ampEnvelope_.getLevel();

            const float noteRatio = semitoneRatio (static_cast<float> (midiNote_) - 60.0f);
            engine_.setPitchRatio (basePitchRatio_ * noteRatio * pitchEnv);

            const float osc = engine_.renderSample();
            const float shaped = osc * ampEnv * patch_.level;

            // Phase 2: TVF ZDF + saturation; cutoffEnv cached for filter coefficient updates.
            lastCutoffNorm_ = cutoffEnv;
            output[i] = shaped;

            if (! ampEnvelope_.isActive())
                active_ = false;
        }
    }

    float getLastCutoffNorm() const noexcept { return lastCutoffNorm_; }

private:
    static float semitoneRatio (float semitones) noexcept
    {
        return std::pow (2.0f, semitones / 12.0f);
    }

    double sampleRate_ = 44100.0;
    TonePatch patch_{};
    SampleEngine engine_{};
    RateLevelEnvelope pitchEnvelope_{};
    RateLevelEnvelope cutoffEnvelope_{};
    RateLevelEnvelope ampEnvelope_{};
    float basePitchRatio_ = 1.0f;
    float lastCutoffNorm_ = 1.0f;
    std::uint8_t midiNote_ = 60;
    std::size_t samplesUntilControlTick_ = 0;
    bool active_ = false;
};

} // namespace jdupgraded::dsp
