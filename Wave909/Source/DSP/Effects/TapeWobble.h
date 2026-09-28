#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace wave909::dsp
{

/** Macro: wow/flutter pitch drift + noise floor + sub detune coupling. */
class TapeWobble final
{
public:
    void prepare (double sampleRate) noexcept
    {
        sampleRate_ = sampleRate > 0.0 ? sampleRate : 44100.0;
    }

    void reset() noexcept
    {
        wowPhase_ = 0.0;
        flutterPhase_ = 0.0;
    }

    void setMacro (float macro01) noexcept
    {
        macro_ = std::clamp (macro01, 0.0f, 1.0f);
    }

    float pitchMultiplier() noexcept
    {
        advancePhases();
        const float wow = std::sin (static_cast<float> (wowPhase_)) * macro_ * 0.012f;
        const float flutter = std::sin (static_cast<float> (flutterPhase_)) * macro_ * 0.004f;
        return 1.0f + wow + flutter;
    }

    float subDetuneCents() const noexcept
    {
        return macro_ * 18.0f;
    }

    float noiseGain (float velocity) const noexcept
    {
        return macro_ * macro_ * 0.08f * velocity;
    }

    float nextNoiseSample (std::uint32_t& rng) noexcept
    {
        rng = rng * 1664525u + 1013904223u;
        const float u = static_cast<float> (rng & 0xffff) / 65535.0f;
        return (u * 2.0f - 1.0f) * noiseGain (1.0f);
    }

private:
    void advancePhases() noexcept
    {
        const double wowHz = 0.15 + static_cast<double> (macro_) * 0.55;
        const double flutterHz = 4.0 + static_cast<double> (macro_) * 9.0;
        wowPhase_ += juceTwoPi * wowHz / sampleRate_;
        flutterPhase_ += juceTwoPi * flutterHz / sampleRate_;
        if (wowPhase_ > juceTwoPi)
            wowPhase_ -= juceTwoPi;
        if (flutterPhase_ > juceTwoPi)
            flutterPhase_ -= juceTwoPi;
    }

    static constexpr double juceTwoPi = 6.283185307179586;

    double sampleRate_ = 44100.0;
    double wowPhase_ = 0.0;
    double flutterPhase_ = 0.0;
    float macro_ = 0.0f;
};

} // namespace wave909::dsp
