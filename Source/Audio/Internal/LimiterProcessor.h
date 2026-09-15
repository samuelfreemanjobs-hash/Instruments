#pragma once

#include "InternalMixProcessor.h"
#include <juce_dsp/juce_dsp.h>

namespace vmpc::audio::internal
{
class LimiterProcessor : public InternalMixProcessor
{
public:
    LimiterProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

private:
    juce::AudioParameterFloat* ceiling = nullptr;
    juce::AudioParameterFloat* release = nullptr;

    juce::dsp::Limiter<float> limiter;
    juce::dsp::ProcessSpec spec {};
};
} // namespace vmpc::audio::internal
