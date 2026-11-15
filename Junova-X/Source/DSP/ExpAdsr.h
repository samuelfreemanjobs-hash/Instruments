#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace junovax::dsp
{
/** Exponential ADSR (Juno-style punch, click-aware retrigger). */
class ExpAdsr
{
public:
    void setSampleRate (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
        recalcCoeffs();
    }

    void setParameters (float attackSec, float decaySec, float sustain, float releaseSec) noexcept
    {
        attackSec = juce::jmax (0.0005f, attackSec);
        decaySec = juce::jmax (0.0005f, decaySec);
        releaseSec = juce::jmax (0.0005f, releaseSec);
        sustainLevel_ = juce::jlimit (0.0f, 1.0f, sustain);

        attackSec_ = attackSec;
        decaySec_ = decaySec;
        releaseSec_ = releaseSec;
        recalcCoeffs();
    }

    void reset() noexcept
    {
        stage_ = Stage::idle;
        level_ = 0.0f;
    }

    bool isActive() const noexcept
    {
        return stage_ != Stage::idle;
    }

    void noteOn() noexcept
    {
        stage_ = Stage::attack;
        if (level_ < 1.0e-5f)
            level_ = 0.0f;
    }

    void noteOff() noexcept
    {
        if (stage_ != Stage::idle)
            stage_ = Stage::release;
    }

    float getNextSample() noexcept
    {
        switch (stage_)
        {
            case Stage::attack:
            {
                level_ += (1.0f - level_) * attackCoeff_;
                if (level_ > 0.9995f)
                {
                    level_ = 1.0f;
                    stage_ = Stage::decay;
                }
                break;
            }
            case Stage::decay:
            {
                level_ += (sustainLevel_ - level_) * decayCoeff_;
                if (std::abs (level_ - sustainLevel_) < 1.0e-4f)
                {
                    level_ = sustainLevel_;
                    stage_ = Stage::sustain;
                }
                break;
            }
            case Stage::sustain:
                break;
            case Stage::release:
            {
                level_ += (0.0f - level_) * releaseCoeff_;
                if (level_ < 1.0e-5f)
                {
                    level_ = 0.0f;
                    stage_ = Stage::idle;
                }
                break;
            }
            case Stage::idle:
            default:
                level_ = 0.0f;
                break;
        }
        return level_;
    }

private:
    enum class Stage
    {
        idle,
        attack,
        decay,
        sustain,
        release
    };

    void recalcCoeffs() noexcept
    {
        const float sr = static_cast<float> (sampleRate_);
        attackCoeff_ = 1.0f - std::exp (-1.0f / (attackSec_ * sr));
        decayCoeff_ = 1.0f - std::exp (-1.0f / (decaySec_ * sr));
        releaseCoeff_ = 1.0f - std::exp (-1.0f / (releaseSec_ * sr));
    }

    double sampleRate_ = 44100.0;
    float attackSec_ = 0.01f;
    float decaySec_ = 0.2f;
    float releaseSec_ = 0.4f;
    float sustainLevel_ = 0.75f;
    float attackCoeff_ = 0.01f;
    float decayCoeff_ = 0.01f;
    float releaseCoeff_ = 0.01f;
    Stage stage_ = Stage::idle;
    float level_ = 0.0f;
};
} // namespace junovax::dsp