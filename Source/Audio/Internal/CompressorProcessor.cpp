#include "CompressorProcessor.h"

namespace resonance::audio::internal
{
CompressorProcessor::CompressorProcessor()
    : InternalMixProcessor("Resonance Compressor")
{
    addParameter(threshold = new juce::AudioParameterFloat(
        juce::ParameterID { "threshold", 1 },
        "Threshold",
        juce::NormalisableRange<float> { -60.0f, 0.0f, 0.1f },
        -18.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    addParameter(ratio = new juce::AudioParameterFloat(
        juce::ParameterID { "ratio", 1 },
        "Ratio",
        juce::NormalisableRange<float> { 1.0f, 20.0f, 0.1f },
        3.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(
            [](float v, int) { return juce::String(v, 1) + ":1"; })));
    addParameter(attack = new juce::AudioParameterFloat(
        juce::ParameterID { "attack", 1 },
        "Attack",
        juce::NormalisableRange<float> { 1.0f, 100.0f, 0.1f },
        10.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));
    addParameter(release = new juce::AudioParameterFloat(
        juce::ParameterID { "release", 1 },
        "Release",
        juce::NormalisableRange<float> { 10.0f, 500.0f, 1.0f },
        120.0f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));
    addParameter(makeup = new juce::AudioParameterFloat(
        juce::ParameterID { "makeup", 1 },
        "Makeup",
        juce::NormalisableRange<float> { 0.0f, 24.0f, 0.1f },
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
}

void CompressorProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;
    compressor.prepare(spec);
}

void CompressorProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    compressor.setThreshold(threshold->get());
    compressor.setRatio(ratio->get());
    compressor.setAttack(attack->get() / 1000.0f);
    compressor.setRelease(release->get() / 1000.0f);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    compressor.process(context);

    const float makeupGain = juce::Decibels::decibelsToGain(makeup->get());
    buffer.applyGain(makeupGain);
}

juce::AudioProcessorEditor* CompressorProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}
} // namespace resonance::audio::internal
