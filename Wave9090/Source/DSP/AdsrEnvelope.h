#pragma once

#include <algorithm>
#include <cmath>

namespace wave9090::dsp
{

/** Exponential ADSR with optional soft attack floor for click-free note on. */
class AdsrEnvelope final
{
public:
    enum class Stage { idle, attack, decay, sustain, release };

    void setSampleRate (double sr) noexcept { sampleRate_ = sr > 0.0 ? sr : 44100.0; updateCoeffs(); }

    void setParams (float a, float d, float s, float r, float curve = 1.0f) noexcept
    {
        attackSec_ = std::max (0.001f, a);
        decaySec_ = std::max (0.001f, d);
        sustain_ = std::clamp (s, 0.0f, 1.0f);
        releaseSec_ = std::max (0.001f, r);
        curve_ = std::clamp (curve, 0.35f, 4.0f);
        updateCoeffs();
    }

    void noteOn() noexcept
    {
        if (stage_ == Stage::idle && level_ <= 1.0e-6f)
            level_ = 1.0e-6f;
        else if (stage_ == Stage::release || stage_ == Stage::sustain || stage_ == Stage::decay)
            level_ = std::max (level_, 1.0e-5f);
        stage_ = Stage::attack;
    }

    void noteOff() noexcept
    {
        if (stage_ != Stage::idle)
            stage_ = Stage::release;
    }

    void reset() noexcept
    {
        stage_ = Stage::idle;
        level_ = 0.0f;
    }

    bool isActive() const noexcept { return stage_ != Stage::idle; }
    float currentLevel() const noexcept { return level_; }

    float process() noexcept
    {
        switch (stage_)
        {
            case Stage::attack:
                level_ += (1.0f - level_) * attackCoef_;
                if (level_ >= 0.9995f)
                {
                    level_ = 1.0f;
                    stage_ = Stage::decay;
                }
                break;
            case Stage::decay:
                level_ += (sustain_ - level_) * decayCoef_;
                if (std::abs (level_ - sustain_) < 1.0e-4f)
                {
                    level_ = sustain_;
                    stage_ = Stage::sustain;
                }
                break;
            case Stage::sustain:
                break;
            case Stage::release:
                level_ += (0.0f - level_) * releaseCoef_;
                if (level_ <= 1.0e-5f)
                {
                    level_ = 0.0f;
                    stage_ = Stage::idle;
                }
                break;
            case Stage::idle:
                level_ = 0.0f;
                break;
        }
        const float out = std::pow (std::clamp (level_, 0.0f, 1.0f), 1.0f / curve_);
        return out;
    }

private:
    void updateCoeffs() noexcept
    {
        auto coef = [this] (float seconds) -> float
        {
            return 1.0f - std::exp (-1.0f / (seconds * static_cast<float> (sampleRate_)));
        };
        attackCoef_ = coef (attackSec_);
        decayCoef_ = coef (decaySec_);
        releaseCoef_ = coef (releaseSec_);
    }

    double sampleRate_ = 44100.0;
    float attackSec_ = 0.01f;
    float decaySec_ = 0.2f;
    float sustain_ = 0.8f;
    float releaseSec_ = 0.4f;
    float curve_ = 1.2f;
    float attackCoef_ = 0.01f;
    float decayCoef_ = 0.01f;
    float releaseCoef_ = 0.01f;
    float level_ = 0.0f;
    Stage stage_ = Stage::idle;
};

} // namespace wave9090::dsp