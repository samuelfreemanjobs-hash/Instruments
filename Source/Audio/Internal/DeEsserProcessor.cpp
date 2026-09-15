#include "DeEsserProcessor.h"

namespace resonance::audio::internal
{
DeEsserProcessor::DeEsserProcessor()
    : InternalMixProcessor("Resonance De-Esser")
{
    addParameter(thresholdDb = new juce::AudioParameterFloat(
        juce::ParameterID { "threshold", 1 },
        "Threshold",
        juce::NormalisableRange<float> { -40.0f, 0.0f, 0.1f },
        -18.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    addParameter(amount = new juce::AudioParameterFloat(
        juce::ParameterID { "amount", 1 },
        "Amount",
        juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f },
        0.6f));
}

void DeEsserProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 1;
    compressor.prepare(spec);

    for (auto& f : bandPass)
        f.prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 1u });

    const auto coeffs = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 6500.0f, 1.2f);
    for (auto& f : bandPass)
        *f.state = *coeffs;

    bandBuffer.setSize(2, samplesPerBlock);
}

void DeEsserProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int channels = juce::jmin(2, buffer.getNumChannels());
    const int numSamples = buffer.getNumSamples();
    bandBuffer.setSize(channels, numSamples, false, false, true);

    const float mix = amount->get();
    compressor.setThreshold(thresholdDb->get());
    compressor.setRatio(6.0f);
    compressor.setAttack(0.001f);
    compressor.setRelease(0.05f);

    for (int ch = 0; ch < channels; ++ch)
    {
        bandBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

        juce::dsp::AudioBlock<float> block(bandBuffer.getArrayOfWritePointers() + ch, 1,
                                           static_cast<size_t>(numSamples));
        juce::dsp::ProcessContextReplacing<float> filterCtx(block);
        bandPass[ch].process(filterCtx);

        juce::dsp::ProcessContextReplacing<float> compCtx(block);
        compressor.process(compCtx);

        bandBuffer.applyGain(ch, 0, numSamples, -mix);
        buffer.addFrom(ch, 0, bandBuffer, ch, 0, numSamples);
    }
}

juce::AudioProcessorEditor* DeEsserProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}
} // namespace resonance::audio::internal
