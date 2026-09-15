#pragma once

// Phase 2: Zero-delay feedback TVF with nonlinear resonance limiting (JD character).

namespace jdupgraded::dsp
{

class ZdfTvf final
{
public:
    void prepare (double sampleRate) noexcept { sampleRate_ = sampleRate; }

    float process (float input, float cutoffNorm) noexcept
    {
        // Placeholder pass-through until phase 2 coefficient + saturation model lands.
        return input * cutoffNorm;
    }

private:
    double sampleRate_ = 44100.0;
};

} // namespace jdupgraded::dsp
