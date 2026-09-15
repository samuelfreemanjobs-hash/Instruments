#include "GainProcessor.h"

namespace resonance::audio::internal
{
GainProcessor::GainProcessor()
    : InternalMixProcessor("Resonance Gain")
{
    addParameter(gainDb = new juce::AudioParameterFloat(
        juce::ParameterID { "gainDb", 1 },
        "Gain",
        juce::NormalisableRange<float> { -60.0f, 12.0f, 0.01f },
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
}

void GainProcessor::prepareToPlay(double sampleRate, int)
{
    gainLinear.reset(sampleRate, 0.02);
    gainLinear.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(gainDb->get()));
}

void GainProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    gainLinear.setTargetValue(juce::Decibels::decibelsToGain(gainDb->get()));

    const int numSamples = buffer.getNumSamples();
    for (int i = 0; i < numSamples; ++i)
    {
        const float g = gainLinear.getNextValue();
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.getWritePointer(ch)[i] *= g;
    }
}

juce::AudioProcessorEditor* GainProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}
} // namespace resonance::audio::internal
