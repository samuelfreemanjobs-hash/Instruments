#include "TapeSatProcessor.h"

namespace resonance::audio::internal
{
TapeSatProcessor::TapeSatProcessor()
    : InternalMixProcessor("Resonance Tape")
{
    addParameter(drive = new juce::AudioParameterFloat(
        juce::ParameterID { "drive", 1 },
        "Drive",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f },
        0.2f));
}

void TapeSatProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    const float d = drive->get();
    const float gain = 1.0f + d * 4.0f;

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            data[i] = std::tanh(data[i] * gain) * (1.0f - d * 0.15f);
    }
}

juce::AudioProcessorEditor* TapeSatProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}
} // namespace resonance::audio::internal
