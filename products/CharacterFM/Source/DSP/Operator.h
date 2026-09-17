#pragma once

#include <cmath>

namespace characterfm::dsp
{

struct OperatorParams
{
    float ratio = 1.0f;
    float level = 0.8f;
    float attackSec = 0.01f;
    float decaySec = 0.2f;
    float sustain = 0.7f;
    float releaseSec = 0.3f;
    float feedback = 0.0f;
};

class Operator
{
public:
    void reset (double sampleRate) noexcept;
    void start (float velocity) noexcept;
    void stop() noexcept;
    bool isActive() const noexcept { return stage_ != Stage::idle; }

    void setParams (const OperatorParams& p) noexcept;

    /** Phase modulation input (radians scale, scaled by mod depth in voice). */
    float render (float phaseMod) noexcept;

private:
    enum class Stage { idle, attack, decay, sustain, release };

    OperatorParams params_{};
    double sampleRate_ = 44100.0;
    float phase_ = 0.0f;
    float phaseInc_ = 0.0f;
    float envelope_ = 0.0f;
    float velocity_ = 1.0f;
    Stage stage_ = Stage::idle;
    float baseFrequencyHz_ = 440.0f;

    void advanceEnvelope() noexcept;
    void updatePhaseIncrement() noexcept;

public:
    void setBaseFrequency (float hz) noexcept
    {
        baseFrequencyHz_ = hz;
        updatePhaseIncrement();
    }

    float getLastOutput() const noexcept { return lastOutput_; }

private:
    float lastOutput_ = 0.0f;
};

} // namespace characterfm::dsp
