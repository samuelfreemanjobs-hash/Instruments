#pragma once

#include "InternalMixProcessor.h"

namespace vmpc::audio::internal
{
class GainProcessor : public InternalMixProcessor
{
public:
    GainProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

private:
    juce::AudioParameterFloat* gainDb = nullptr;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gainLinear;
};
} // namespace vmpc::audio::internal
