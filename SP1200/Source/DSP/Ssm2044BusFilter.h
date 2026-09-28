#pragma once

#include <juce_dsp/juce_dsp.h>

namespace sp1200
{
/** Stereo bus low-pass (SSM2044-inspired ladder). */
class Ssm2044BusFilter
{
public:
    void prepare (double sampleRate, int samplesPerBlock);
    void reset();

    void setBaseCutoffNorm (float norm);
    void setResonance (float norm);
    /** Per-step hit mod: 0..1 maps to cutoff offset. */
    void pushStepModulation (float stepFilter01);
    void beginBlock();

    void process (juce::AudioBuffer<float>& buffer);

private:
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> filterL_;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> filterR_;

    double sampleRate_ = 44100.0;
    float baseCutoffNorm_ = 0.72f;
    float resonance_ = 0.35f;
    float modOffset_ = 0.0f;
    float modSmoothed_ = 0.0f;

    void updateCoefficients();
};

} // namespace sp1200
