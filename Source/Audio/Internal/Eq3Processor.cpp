#include "Eq3Processor.h"

namespace vmpc::audio::internal
{
Eq3Processor::Eq3Processor()
    : InternalMixProcessor("VMPC EQ-3")
{
    addParameter(lowGain = new juce::AudioParameterFloat(
        juce::ParameterID { "low", 1 },
        "Low",
        juce::NormalisableRange<float> { -12.0f, 12.0f, 0.1f },
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    addParameter(midGain = new juce::AudioParameterFloat(
        juce::ParameterID { "mid", 1 },
        "Mid",
        juce::NormalisableRange<float> { -12.0f, 12.0f, 0.1f },
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
    addParameter(highGain = new juce::AudioParameterFloat(
        juce::ParameterID { "high", 1 },
        "High",
        juce::NormalisableRange<float> { -12.0f, 12.0f, 0.1f },
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));
}

void Eq3Processor::prepareToPlay(double sampleRate, int)
{
    sampleRateHz = sampleRate;
    for (int ch = 0; ch < 2; ++ch)
    {
        lowShelf[ch].prepare({ sampleRate, 512u, 1u });
        midPeak[ch].prepare({ sampleRate, 512u, 1u });
        highShelf[ch].prepare({ sampleRate, 512u, 1u });
    }
    updateFilters();
}

void Eq3Processor::updateFilters()
{
    const auto lowCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRateHz, 120.0f, 0.707f, juce::Decibels::decibelsToGain(lowGain->get()));
    const auto midCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRateHz, 1200.0f, 0.9f, juce::Decibels::decibelsToGain(midGain->get()));
    const auto highCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRateHz, 8000.0f, 0.707f, juce::Decibels::decibelsToGain(highGain->get()));

    for (int ch = 0; ch < 2; ++ch)
    {
        *lowShelf[ch].state = *lowCoeffs;
        *midPeak[ch].state = *midCoeffs;
        *highShelf[ch].state = *highCoeffs;
    }
}

void Eq3Processor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    updateFilters();

    const int channels = juce::jmin(2, buffer.getNumChannels());

    for (int ch = 0; ch < channels; ++ch)
    {
        juce::dsp::AudioBlock<float> block(buffer.getArrayOfWritePointers() + ch, 1, buffer.getNumSamples());
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        lowShelf[ch].process(ctx);
        midPeak[ch].process(ctx);
        highShelf[ch].process(ctx);
    }
}

juce::AudioProcessorEditor* Eq3Processor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}
} // namespace vmpc::audio::internal
