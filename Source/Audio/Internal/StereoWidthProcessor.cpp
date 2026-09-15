#include "StereoWidthProcessor.h"

namespace resonance::audio::internal
{
StereoWidthProcessor::StereoWidthProcessor()
    : InternalMixProcessor("Resonance Width")
{
    addParameter(width = new juce::AudioParameterFloat(
        juce::ParameterID { "width", 1 },
        "Width",
        juce::NormalisableRange<float> { 0.0f, 2.0f, 0.01f },
        1.0f));
}

void StereoWidthProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    if (buffer.getNumChannels() < 2)
        return;

    const float w = width->get();
    auto* l = buffer.getWritePointer(0);
    auto* r = buffer.getWritePointer(1);
    const int n = buffer.getNumSamples();

    for (int i = 0; i < n; ++i)
    {
        const float mid = 0.5f * (l[i] + r[i]);
        const float side = 0.5f * (l[i] - r[i]) * w;
        l[i] = mid + side;
        r[i] = mid - side;
    }
}

juce::AudioProcessorEditor* StereoWidthProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}
} // namespace resonance::audio::internal
