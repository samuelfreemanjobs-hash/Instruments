#include "Ssm2044BusFilter.h"

#include <algorithm>
#include <cmath>

namespace sp1200
{
void Ssm2044BusFilter::prepare (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    sampleRate_ = sampleRate;
    juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (samplesPerBlock), 1 };
    filterL_.prepare (spec);
    filterR_.prepare (spec);
    reset();
    updateCoefficients();
}

void Ssm2044BusFilter::reset()
{
    filterL_.reset();
    filterR_.reset();
    modSmoothed_ = 0.0f;
}

void Ssm2044BusFilter::setBaseCutoffNorm (float norm)
{
    baseCutoffNorm_ = std::clamp (norm, 0.0f, 1.0f);
    updateCoefficients();
}

void Ssm2044BusFilter::setResonance (float norm)
{
    resonance_ = std::clamp (norm, 0.0f, 1.0f);
    updateCoefficients();
}

void Ssm2044BusFilter::pushStepModulation (float stepFilter01)
{
    modOffset_ = std::max (modOffset_, (stepFilter01 - 0.5f) * 0.6f);
}

void Ssm2044BusFilter::beginBlock()
{
    modOffset_ = 0.0f;
}

void Ssm2044BusFilter::updateCoefficients()
{
    const float cutoffNorm = std::clamp (baseCutoffNorm_ + modSmoothed_, 0.05f, 1.0f);
    const float hz = 80.0f + std::pow (cutoffNorm, 2.1f) * 11000.0f;
    const float q = 0.5f + resonance_ * 4.0f;
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass (sampleRate_, hz, q);
    *filterL_.state = *coeffs;
    *filterR_.state = *coeffs;
}

void Ssm2044BusFilter::process (juce::AudioBuffer<float>& buffer)
{
    modSmoothed_ += (modOffset_ - modSmoothed_) * 0.15f;
    modOffset_ *= 0.92f;
    updateCoefficients();

    if (buffer.getNumChannels() < 1)
        return;

    juce::dsp::AudioBlock<float> block (buffer);
    if (buffer.getNumChannels() >= 1)
    {
        auto ch0 = block.getSingleChannelBlock (0);
        juce::dsp::ProcessContextReplacing<float> ctx (ch0);
        filterL_.process (ctx);
    }
    if (buffer.getNumChannels() >= 2)
    {
        auto ch1 = block.getSingleChannelBlock (1);
        juce::dsp::ProcessContextReplacing<float> ctx (ch1);
        filterR_.process (ctx);
    }
}

} // namespace sp1200
