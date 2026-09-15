#pragma once

#include "AudioConstants.h"

#include <algorithm>
#include <cmath>

namespace jdupgraded::dsp
{

/** JD-style rate/level envelope: segments advance toward target levels at exponential rates. */
class RateLevelEnvelope final
{
public:
    void reset (float initialLevel = 0.0f) noexcept
    {
        level_ = initialLevel;
        targetLevel_ = initialLevel;
        rateCoeff_ = 1.0f;
        gate_ = false;
        stage_ = Stage::idle;
    }

    void setSampleRate (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
        controlSamples_ = static_cast<std::size_t> (std::max (1.0, sampleRate_ / static_cast<double> (kControlRateDivisor)));
        recalcRateCoeff();
    }

    void noteOn (float attackLevel, float attackTimeSec, float decayLevel, float decayTimeSec, float sustainLevel) noexcept
    {
        sustainLevel_ = clampLevel (sustainLevel);
        gate_ = true;
        stage_ = Stage::attack;
        targetLevel_ = clampLevel (attackLevel);
        setSegmentRate (attackTimeSec);

        pendingDecayLevel_ = clampLevel (decayLevel);
        pendingDecayTime_ = decayTimeSec;
    }

    void noteOff (float releaseTimeSec) noexcept
    {
        gate_ = false;
        stage_ = Stage::release;
        targetLevel_ = 0.0f;
        setSegmentRate (releaseTimeSec);
    }

    void advanceControlTick() noexcept
    {
        const float epsilon = 1.0e-5f;
        if (std::abs (level_ - targetLevel_) <= epsilon)
        {
            level_ = targetLevel_;
            advanceStage();
        }
        else
        {
            level_ += (targetLevel_ - level_) * rateCoeff_;
        }
    }

    float getLevel() const noexcept { return level_; }
    bool isActive() const noexcept { return stage_ != Stage::idle || level_ > 1.0e-6f; }

private:
    enum class Stage : std::uint8_t
    {
        idle,
        attack,
        decay,
        sustain,
        release
    };

    static float clampLevel (float v) noexcept
    {
        return std::clamp (v, 0.0f, 1.0f);
    }

    void setSegmentRate (float timeSec) noexcept
    {
        timeSec = std::clamp (timeSec, kMinEnvelopeTimeSec, kMaxEnvelopeTimeSec);
        segmentTimeSec_ = timeSec;
        recalcRateCoeff();
    }

    void recalcRateCoeff() noexcept
    {
        const double ticks = (segmentTimeSec_ * sampleRate_) / static_cast<double> (controlSamples_);
        const double n = std::max (1.0, ticks);
        rateCoeff_ = static_cast<float> (1.0 - std::exp (-1.0 / n));
    }

    void advanceStage() noexcept
    {
        switch (stage_)
        {
            case Stage::attack:
                stage_ = Stage::decay;
                targetLevel_ = pendingDecayLevel_;
                setSegmentRate (pendingDecayTime_);
                break;
            case Stage::decay:
                stage_ = Stage::sustain;
                targetLevel_ = sustainLevel_;
                rateCoeff_ = 0.0f;
                break;
            case Stage::sustain:
                if (! gate_)
                {
                    stage_ = Stage::release;
                    targetLevel_ = 0.0f;
                    setSegmentRate (pendingReleaseTime_);
                }
                break;
            case Stage::release:
                stage_ = Stage::idle;
                level_ = 0.0f;
                break;
            case Stage::idle:
            default:
                break;
        }
    }

    void setReleaseTime (float releaseTimeSec) noexcept { pendingReleaseTime_ = releaseTimeSec; }

public:
    void configureRelease (float releaseTimeSec) noexcept { pendingReleaseTime_ = releaseTimeSec; }

private:
    double sampleRate_ = 44100.0;
    std::size_t controlSamples_ = 1378;
    float level_ = 0.0f;
    float targetLevel_ = 0.0f;
    float rateCoeff_ = 1.0f;
    float segmentTimeSec_ = 0.01f;
    float sustainLevel_ = 1.0f;
    float pendingDecayLevel_ = 0.8f;
    float pendingDecayTime_ = 0.2f;
    float pendingReleaseTime_ = 0.3f;
    bool gate_ = false;
    Stage stage_ = Stage::idle;
};

} // namespace jdupgraded::dsp
