#pragma once

#include <algorithm>
#include <cmath>

namespace disklordz::rompler::dsp
{

class AdsrEnvelope final
{
public:
    void setSampleRate (double sr) noexcept { sampleRate_ = sr > 0.0 ? sr : 44100.0; }

    void setParams (float a, float d, float s, float r) noexcept
    {
        attack_ = std::max (0.001f, a);
        decay_ = std::max (0.001f, d);
        sustain_ = std::clamp (s, 0.0f, 1.0f);
        release_ = std::max (0.001f, r);
    }

    void noteOn() noexcept
    {
        state_ = State::attack;
        level_ = 0.0f;
    }

    void noteOff() noexcept
    {
        if (state_ != State::idle)
            state_ = State::release;
    }

    void reset() noexcept
    {
        state_ = State::idle;
        level_ = 0.0f;
    }

    bool isActive() const noexcept { return state_ != State::idle; }

    float process() noexcept
    {
        switch (state_)
        {
            case State::attack:
                level_ += 1.0f / (attack_ * static_cast<float> (sampleRate_));
                if (level_ >= 1.0f)
                {
                    level_ = 1.0f;
                    state_ = State::decay;
                }
                break;
            case State::decay:
                level_ += (sustain_ - level_) / (decay_ * static_cast<float> (sampleRate_));
                if (std::abs (level_ - sustain_) < 1.0e-4f)
                {
                    level_ = sustain_;
                    state_ = State::sustain;
                }
                break;
            case State::sustain:
                break;
            case State::release:
                level_ += (0.0f - level_) / (release_ * static_cast<float> (sampleRate_));
                if (level_ <= 1.0e-5f)
                {
                    level_ = 0.0f;
                    state_ = State::idle;
                }
                break;
            case State::idle:
                level_ = 0.0f;
                break;
        }
        return level_;
    }

private:
    enum class State
    {
        idle,
        attack,
        decay,
        sustain,
        release
    };

    double sampleRate_ = 44100.0;
    float attack_ = 0.01f;
    float decay_ = 0.2f;
    float sustain_ = 0.8f;
    float release_ = 0.3f;
    float level_ = 0.0f;
    State state_ = State::idle;
};

} // namespace disklordz::rompler::dsp
