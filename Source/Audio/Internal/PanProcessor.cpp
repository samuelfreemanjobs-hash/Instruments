#include "PanProcessor.h"

namespace vmpc::audio::internal
{
PanProcessor::PanProcessor()
    : InternalMixProcessor("VMPC Pan")
{
    addParameter(pan = new juce::AudioParameterFloat(
        juce::ParameterID { "pan", 1 },
        "Pan",
        juce::NormalisableRange<float> { -1.0f, 1.0f, 0.001f },
        0.0f));
}

void PanProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    if (buffer.getNumChannels() < 2)
        return;

    const float p = pan->get();
    const float angle = (p + 1.0f) * 0.25f * juce::MathConstants<float>::pi;
    const float gainL = std::cos(angle);
    const float gainR = std::sin(angle);

    buffer.applyGain(0, 0, buffer.getNumSamples(), gainL);
    buffer.applyGain(1, 0, buffer.getNumSamples(), gainR);
}

juce::AudioProcessorEditor* PanProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}
} // namespace vmpc::audio::internal
