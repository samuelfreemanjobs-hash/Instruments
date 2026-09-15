#pragma once

#include "InternalMixProcessor.h"

namespace resonance::audio::internal
{
class TapeSatProcessor : public InternalMixProcessor
{
public:
    TapeSatProcessor();

    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

private:
    juce::AudioParameterFloat* drive = nullptr;
};
} // namespace resonance::audio::internal
