#pragma once

#include <algorithm>
#include <cmath>

namespace aftrdark::dsp
{

class AdsrEnvelope final
{
public:
    enum class Stage { idle, attack, decay, sustain, release };

    void setSampleRate (double sr) noexcept { sampleRate_ = sr > 0.0 ? sr : 44100.0; }

    void setParams (float a, float d, float s, float r) noexcept
    {
        attackSec_ = std::max (0.001f, a);
        decaySec_ = std::max (0.001f, d);
        sustain_ = std::clamp (s, 0.0f, 1.0f);
        releaseSec_ = std::max (0.001f, r);
    }

    void noteOn() noexcept
    {
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

    float process() noexcept
    {
        switch (stage_)
        {
            case Stage::attack:
                level_ += 1.0f / (attackSec_ * static_cast<float> (sampleRate_));
                if (level_ >= 1.0f)
                {
                    level_ = 1.0f;
                    stage_ = Stage::decay;
                }
                break;
            case Stage::decay:
                level_ -= (1.0f - sustain_) / (decaySec_ * static_cast<float> (sampleRate_));
                if (level_ <= sustain_)
                {
                    level_ = sustain_;
                    stage_ = Stage::sustain;
                }
                break;
            case Stage::sustain:
                break;
            case Stage::release:
                level_ -= sustain_ / (releaseSec_ * static_cast<float> (sampleRate_));
                if (level_ <= 0.0f)
                {
                    level_ = 0.0f;
                    stage_ = Stage::idle;
                }
                break;
            case Stage::idle:
                level_ = 0.0f;
                break;
        }
        return level_;
    }

private:
    double sampleRate_ = 44100.0;
    float attackSec_ = 0.01f;
    float decaySec_ = 0.2f;
    float sustain_ = 0.8f;
    float releaseSec_ = 0.4f;
    float level_ = 0.0f;
    Stage stage_ = Stage::idle;
};

} // namespace aftrdark::dsp
