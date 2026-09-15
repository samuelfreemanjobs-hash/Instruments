#pragma once

#include "InternalMixProcessor.h"
#include <juce_dsp/juce_dsp.h>

namespace vmpc::audio::internal
{
class HpfProcessor : public InternalMixProcessor
{
public:
    HpfProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

private:
    void updateFilter();

    juce::AudioParameterFloat* cutoffHz = nullptr;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter[2];
    double sampleRateHz = 44100.0;
};
} // namespace vmpc::audio::internal
