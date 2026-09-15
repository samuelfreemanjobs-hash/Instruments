#pragma once

#include "InternalMixProcessor.h"
#include <juce_dsp/juce_dsp.h>

namespace resonance::audio::internal
{
class DeEsserProcessor : public InternalMixProcessor
{
public:
    DeEsserProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

private:
    juce::AudioParameterFloat* thresholdDb = nullptr;
    juce::AudioParameterFloat* amount = nullptr;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> bandPass[2];
    juce::dsp::Compressor<float> compressor;
    juce::dsp::ProcessSpec spec {};
    juce::AudioBuffer<float> bandBuffer;
};
} // namespace resonance::audio::internal
