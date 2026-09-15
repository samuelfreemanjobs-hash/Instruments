#include "HpfProcessor.h"

namespace vmpc::audio::internal
{
HpfProcessor::HpfProcessor()
    : InternalMixProcessor("VMPC HPF")
{
    addParameter(cutoffHz = new juce::AudioParameterFloat(
        juce::ParameterID { "cutoff", 1 },
        "Cutoff",
        juce::NormalisableRange<float> { 20.0f, 500.0f, 0.1f, 0.35f },
        80.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));
}

void HpfProcessor::prepareToPlay(double sampleRate, int)
{
    sampleRateHz = sampleRate;
    for (auto& f : filter)
        f.prepare({ sampleRate, 512u, 1u });
    updateFilter();
}

void HpfProcessor::updateFilter()
{
    const auto coeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRateHz, cutoffHz->get());
    for (auto& f : filter)
        *f.state = *coeffs;
}

void HpfProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    updateFilter();

    const int channels = juce::jmin(2, buffer.getNumChannels());
    const int numSamples = buffer.getNumSamples();

    for (int ch = 0; ch < channels; ++ch)
    {
        juce::dsp::AudioBlock<float> block(buffer.getArrayOfWritePointers() + ch, 1,
                                           static_cast<size_t>(numSamples));
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        filter[ch].process(ctx);
    }
}

juce::AudioProcessorEditor* HpfProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}
} // namespace vmpc::audio::internal
