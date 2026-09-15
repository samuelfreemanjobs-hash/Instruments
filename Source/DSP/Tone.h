#pragma once

#include "AudioConstants.h"
#include "Filter/ZdfTvf.h"
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
    float filterResonanceNorm = 0.35f;
    float filterAttackTimeSec = 0.005f;
    float filterDecayTimeSec = 0.2f;
    float filterSustainLevel = 0.7f;
    float filterReleaseTimeSec = 0.3f;
    float ringModAmount = 0.0f;
    float phaseModDepth = 0.0f;
};

class Tone final
{
public:
    SampleEngine& getEngine() noexcept { return engine_; }
    const SampleEngine& getEngine() const noexcept { return engine_; }

    void setSampleRate (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate;
        pitchEnvelope_.setSampleRate (sampleRate);
        cutoffEnvelope_.setSampleRate (sampleRate);
        ampEnvelope_.setSampleRate (sampleRate);
        filter_.prepare (sampleRate);
    }

    void applyPatch (const TonePatch& patch) noexcept
    {
        patch_ = patch;
        if (patch.waveform != nullptr)
            engine_.setWaveform (*patch.waveform);

        const float rootRatio = semitoneRatio (patch.coarseSemis + patch.fineCents / 100.0f);
        basePitchRatio_ = rootRatio;
        engine_.setPitchRatio (basePitchRatio_);
        engine_.setPhaseModDepth (patch.phaseModDepth);
    }

    void setHardSyncMaster (const SampleEngine* master) noexcept
    {
        engine_.setHardSyncFrom (master);
    }

    void start (std::uint8_t midiNote) noexcept
    {
        midiNote_ = midiNote;
        const float noteRatio = semitoneRatio (static_cast<float> (midiNote) - 60.0f);
        engine_.reset();
        engine_.setPitchRatio (basePitchRatio_ * noteRatio);
        filter_.reset();

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
        lastOsc_ = 0.0f;
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

    /** Oscillator only (before coupling / TVF). */
    float renderOscillatorSample() noexcept
    {
        if (samplesUntilControlTick_ == 0)
        {
            advanceEnvelopes();
            samplesUntilControlTick_ = kControlRateDivisor;
        }
        --samplesUntilControlTick_;

        const float pitchEnv = pitchEnvelope_.getLevel();
        const float noteRatio = semitoneRatio (static_cast<float> (midiNote_) - 60.0f);
        engine_.setPitchRatio (basePitchRatio_ * noteRatio * pitchEnv);
        lastOsc_ = engine_.renderSample();
        return lastOsc_;
    }

    float getLastOscillatorSample() const noexcept { return lastOsc_; }

    void setPhaseModSource (float source) noexcept
    {
        engine_.setPhaseModSource (source);
    }

    float applyFilterAndAmp (float osc, float ringPartner, float ringAmount) noexcept
    {
        const float cutoffEnv = cutoffEnvelope_.getLevel();
        const float ampEnv = ampEnvelope_.getLevel();

        float mixed = osc;
        if (ringAmount > 1.0e-5f)
            mixed = osc * (1.0f - ringAmount) + (osc * ringPartner) * ringAmount;

        const float filtered = filter_.process (mixed, cutoffEnv, patch_.filterResonanceNorm);
        return filtered * ampEnv * patch_.level;
    }

    void advanceEnvelopes() noexcept
    {
        pitchEnvelope_.advanceControlTick();
        cutoffEnvelope_.advanceControlTick();
        ampEnvelope_.advanceControlTick();
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
                advanceEnvelopes();
                samplesUntilControlTick_ = kControlRateDivisor;
            }
            --samplesUntilControlTick_;

            const float osc = renderOscillatorSample();
            output[i] = applyFilterAndAmp (osc, 0.0f, 0.0f);

            if (! ampEnvelope_.isActive())
                active_ = false;
        }
    }

private:
    static float semitoneRatio (float semitones) noexcept
    {
        return std::pow (2.0f, semitones / 12.0f);
    }

    double sampleRate_ = 44100.0;
    TonePatch patch_{};
    SampleEngine engine_{};
    ZdfTvf filter_{};
    RateLevelEnvelope pitchEnvelope_{};
    RateLevelEnvelope cutoffEnvelope_{};
    RateLevelEnvelope ampEnvelope_{};
    float basePitchRatio_ = 1.0f;
    float lastOsc_ = 0.0f;
    std::uint8_t midiNote_ = 60;
    std::size_t samplesUntilControlTick_ = 0;
    bool active_ = false;
};

} // namespace jdupgraded::dsp
