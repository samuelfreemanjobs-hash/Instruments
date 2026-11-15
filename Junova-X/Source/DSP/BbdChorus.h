#pragma once

#include <juce_dsp/juce_dsp.h>

#include <array>

namespace junovax::dsp
{
/** MN3009-style BBD chorus (Juno I / II / I+II). */
class BbdChorus
{
public:
    void prepare (double sampleRate) noexcept;
    void reset() noexcept;

    /** mode 0 off, 1 I, 2 II, 3 I+II (dual BBD). */
    void process (float& left, float& right, int mode) noexcept;

private:
    double sampleRate_ = 48000.0;
    float chorusPhaseI_ = 0.0f;
    float chorusPhaseII_ = 0.0f;
    float wetLpL_ = 0.0f;
    float wetLpR_ = 0.0f;
    float clockNoiseLp_ = 0.0f;
    int delayWrite_ = 0;
    std::array<float, 4096> delayL_{};
    std::array<float, 4096> delayR_{};
    juce::Random rng_;
};
} // namespace junovax::dsp