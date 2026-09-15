#include "LimiterProcessor.h"

namespace vmpc::audio::internal
{
LimiterProcessor::LimiterProcessor()
    : InternalMixProcessor("VMPC Limiter")
{
    addParameter(ceiling = new juce::AudioParameterFloat(
        juce::ParameterID { "ceiling", 1 },
        "Ceiling",
        juce::NormalisableRange<float> { -12.0f, 0.0f, 0.1f },
        -0.3f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    addParameter(release = new juce::AudioParameterFloat(
        juce::ParameterID { "release", 1 },
        "Release",
        juce::NormalisableRange<float> { 1.0f, 200.0f, 0.5f },
        50.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));
}

void LimiterProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;
    limiter.prepare(spec);
}

void LimiterProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    limiter.setThreshold(ceiling->get());
    limiter.setRelease(release->get() / 1000.0f);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    limiter.process(context);
}

juce::AudioProcessorEditor* LimiterProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}
} // namespace vmpc::audio::internal
