#pragma once

#include <JuceHeader.h>

#include <array>

namespace junovax::dsp
{
/** Simple dual-delay chorus (Junova BBD-style placeholder). */
class BbdChorus
{
public:
    void prepare (double sampleRate) noexcept;
    void reset() noexcept;

    /** mode 0 off, 1 I, 2 II, 3 I+II (deeper mod). */
    void process (float& left, float& right, int mode) noexcept;

private:
    double sampleRate_ = 48000.0;
    float chorusPhase_ = 0.0f;
    int delayWrite_ = 0;
    std::array<float, 4096> delayL_{};
    std::array<float, 4096> delayR_{};
};
} // namespace junovax::dsp
