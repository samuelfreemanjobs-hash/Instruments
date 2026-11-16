#pragma once

#include "TracyZone.h"

#include <juce_dsp/juce_dsp.h>

#include <cmath>
#include <utility>

namespace dlz::drum {

// 4x oversampling on JUCE 8.0.15: the factor argument is an exponent (2^2 = 4).
// Buffers are sized in prepare(). process() never resizes them.
class OversampledWaveShaper {
  public:
    void prepare(double newSampleRate, int maximumBlockSize) {
        sampleRate = newSampleRate > 0.0 ? newSampleRate : 48000.0;
        maxBlock = juce::jlimit(1, 8192, maximumBlockSize);
        oversampler.initProcessing(static_cast<size_t>(maxBlock));
        dry.setSize(1, maxBlock, false, false, true);
        wet.setSize(1, maxBlock, false, false, true);
        prepared = true;
    }

    void reset() noexcept {
        if (prepared)
            oversampler.reset();
    }

    int capacity() const noexcept { return maxBlock; }

    int oversamplingFactor() const noexcept { return static_cast<int>(oversampler.getOversamplingFactor()); }

    double oversampledRate() const noexcept {
        return sampleRate * static_cast<double>(juce::jmax(1, oversamplingFactor()));
    }

    // Saturate a base-rate mono buffer. `output` receives `numSamples` samples.
    void processBaseRate(const float *input, float *output, int numSamples, float drive) {
        juce::ScopedNoDenormals noDenormals;
        DLZ_ZONE();
        if (!prepared || input == nullptr || output == nullptr || numSamples <= 0)
            return;

        numSamples = juce::jmin(numSamples, maxBlock);
        dry.copyFrom(0, 0, input, numSamples);
        runSaturation(numSamples, drive);
        juce::FloatVectorOperations::copy(output, wet.getReadPointer(0), numSamples);
    }

    // Synthesize directly into the oversampled block, then saturate and downsample.
    template <typename Writer> void processGenerated(int numSamples, float drive, Writer &&writeUpsampled) {
        juce::ScopedNoDenormals noDenormals;
        DLZ_ZONE();
        if (!prepared || numSamples <= 0)
            return;

        numSamples = juce::jmin(numSamples, maxBlock);
        dry.clear(0, numSamples);
        juce::dsp::AudioBlock<float> block(dry);
        auto input = block.getSubBlock(0, static_cast<size_t>(numSamples));
        auto upsampled = oversampler.processSamplesUp(input);
        if (upsampled.getNumSamples() == 0 || upsampled.getChannelPointer(0) == nullptr)
            return;

        writeUpsampled(upsampled.getChannelPointer(0), static_cast<int>(upsampled.getNumSamples()));
        applyTanh(upsampled.getChannelPointer(0), static_cast<int>(upsampled.getNumSamples()), drive);
        juce::dsp::AudioBlock<float> wetBlock(wet);
        auto output = wetBlock.getSubBlock(0, static_cast<size_t>(numSamples));
        oversampler.processSamplesDown(output);
    }

    const float *wetSamples() const noexcept { return wet.getReadPointer(0); }

  private:
    void runSaturation(int numSamples, float drive) {
        juce::dsp::AudioBlock<float> block(dry);
        auto input = block.getSubBlock(0, static_cast<size_t>(numSamples));
        auto upsampled = oversampler.processSamplesUp(input);
        if (upsampled.getNumSamples() > 0 && upsampled.getChannelPointer(0) != nullptr)
            applyTanh(upsampled.getChannelPointer(0), static_cast<int>(upsampled.getNumSamples()), drive);

        juce::dsp::AudioBlock<float> wetBlock(wet);
        auto output = wetBlock.getSubBlock(0, static_cast<size_t>(numSamples));
        oversampler.processSamplesDown(output);
    }

    static void applyTanh(float *samples, int count, float drive) noexcept {
        const float amount = juce::jmax(1.0f, drive);
        const float normalise = 1.0f / std::tanh(amount);
        for (int i = 0; i < count; ++i)
            samples[i] = std::tanh(samples[i] * amount) * normalise;
    }

    // factor 2 => two half-band stages => 4x. Integer latency keeps one-shot attacks stable.
    juce::dsp::Oversampling<float> oversampler{1, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true,
                                               true};

    juce::AudioBuffer<float> dry;
    juce::AudioBuffer<float> wet;
    double sampleRate = 48000.0;
    int maxBlock = 0;
    bool prepared = false;
};

} // namespace dlz::drum
