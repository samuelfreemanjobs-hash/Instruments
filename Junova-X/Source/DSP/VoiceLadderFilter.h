#pragma once

#include <juce_dsp/juce_dsp.h>

namespace junovax::dsp
{
/** Public wrapper around JUCE Moog ladder (LPF24, self-oscillation at max RES). */
class VoiceLadderFilter final : public juce::dsp::LadderFilter<float>
{
public:
    void prepareFilter (double sampleRate, int maxBlockSize = 512) noexcept
    {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = static_cast<juce::uint32> (maxBlockSize);
        spec.numChannels = 1;
        prepare (spec);
        setMode (juce::dsp::LadderFilterMode::LPF24);
        setDrive (1.05f);
        setResonance (0.15f);
        setCutoffFrequencyHz (1000.0f);
    }

    void setTargets (float cutoffHz, float resonanceNorm, float driveAmt) noexcept
    {
        setCutoffFrequencyHz (juce::jmax (20.0f, cutoffHz));
        setResonance (juce::jlimit (0.0f, 1.0f, resonanceNorm));
        setDrive (juce::jlimit (1.0f, 3.5f, driveAmt));
    }

    float tick (float input) noexcept
    {
        return processSample (input, 0);
    }

    void injectImpulse (float amount) noexcept
    {
        tick (amount);
    }
};
} // namespace junovax::dsp