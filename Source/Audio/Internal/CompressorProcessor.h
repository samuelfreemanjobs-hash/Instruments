#pragma once

#include "InternalMixProcessor.h"
#include <juce_dsp/juce_dsp.h>

namespace resonance::audio::internal
{
class CompressorProcessor : public InternalMixProcessor
{
public:
    CompressorProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

private:
    juce::AudioParameterFloat* threshold = nullptr;
    juce::AudioParameterFloat* ratio = nullptr;
    juce::AudioParameterFloat* attack = nullptr;
    juce::AudioParameterFloat* release = nullptr;
    juce::AudioParameterFloat* makeup = nullptr;

    juce::dsp::Compressor<float> compressor;
    juce::dsp::ProcessSpec spec {};
};
} // namespace resonance::audio::internal
