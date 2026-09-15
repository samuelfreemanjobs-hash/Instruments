#pragma once

#include "InternalMixProcessor.h"
#include <juce_dsp/juce_dsp.h>

namespace resonance::audio::internal
{
class Eq3Processor : public InternalMixProcessor
{
public:
    Eq3Processor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

private:
    void updateFilters();

    juce::AudioParameterFloat* lowGain = nullptr;
    juce::AudioParameterFloat* midGain = nullptr;
    juce::AudioParameterFloat* highGain = nullptr;

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> lowShelf[2];
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> midPeak[2];
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> highShelf[2];
    double sampleRateHz = 44100.0;
};
} // namespace resonance::audio::internal
