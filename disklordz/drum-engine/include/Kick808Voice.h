#pragma once

#include "OversampledWaveShaper.h"

#include <juce_audio_basics/juce_audio_basics.h>

#include <cmath>

namespace dlz::drum {

class Kick808Voice {
  public:
    struct Params {
        float baseFreqHz = 49.0f;
        float pitchEnvHz = 140.0f;
        float pitchDecaySec = 0.045f;
        float ampDecaySec = 0.85f;
        float drive = 2.2f;
        float clickAmount = 0.25f;
    };

    void prepare(const juce::dsp::ProcessSpec &spec) {
        sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 48000.0;
        shaper.prepare(sampleRate, static_cast<int>(spec.maximumBlockSize));
        dry.setSize(1, shaper.capacity(), false, false, true);
        driveSmoothed.reset(sampleRate, 0.01);
        prepared = true;
    }

    void trigger(const Params &newParams, float velocity = 1.0f) {
        params = newParams;
        params.baseFreqHz = juce::jlimit(20.0f, 120.0f, params.baseFreqHz);
        params.pitchEnvHz = juce::jlimit(0.0f, 800.0f, params.pitchEnvHz);
        params.pitchDecaySec = juce::jmax(0.001f, params.pitchDecaySec);
        params.ampDecaySec = juce::jmax(0.005f, params.ampDecaySec);
        params.drive = juce::jmax(1.0f, params.drive);
        currentVelocity = juce::jlimit(0.0f, 1.0f, velocity);
        phase = 0.0;
        elapsedSamples = 0;
        driveSmoothed.setCurrentAndTargetValue(params.drive);
        shaper.reset();
        isRinging = prepared;
    }

    void process(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
        juce::ScopedNoDenormals noDenormals;
        if (!prepared || !isRinging || numSamples <= 0)
            return;

        int remaining = numSamples;
        int offset = startSample;
        while (remaining > 0 && isRinging) {
            const int count = juce::jmin(remaining, shaper.capacity());
            renderChunk(outputBuffer, offset, count);
            remaining -= count;
            offset += count;
        }
    }

    bool active() const noexcept { return isRinging; }
    int scratchCapacity() const noexcept { return shaper.capacity(); }
    int oversamplingFactor() const noexcept { return shaper.oversamplingFactor(); }

  private:
    void renderChunk(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
        auto *dryWrite = dry.getWritePointer(0);
        bool stillRinging = false;

        for (int i = 0; i < numSamples; ++i) {
            const double timeSec = static_cast<double>(elapsedSamples) / sampleRate;
            const double pitchEnv = std::exp(-timeSec / static_cast<double>(params.pitchDecaySec));
            const double ampEnv = std::exp(-timeSec / static_cast<double>(params.ampDecaySec));
            ++elapsedSamples;

            if (ampEnv < 1.0e-5) {
                dryWrite[i] = 0.0f;
                continue;
            }

            stillRinging = true;
            const double instantFreq =
                static_cast<double>(params.baseFreqHz) + static_cast<double>(params.pitchEnvHz) * pitchEnv;
            phase += instantFreq / sampleRate;
            phase -= std::floor(phase);

            double sample = std::sin(phase * juce::MathConstants<double>::twoPi);
            if (timeSec < 0.002)
                sample += static_cast<double>(params.clickAmount) * (1.0 - (timeSec / 0.002));

            dryWrite[i] = static_cast<float>(sample * ampEnv * static_cast<double>(currentVelocity));
        }

        const float drive = juce::jmax(1.0f, driveSmoothed.skip(numSamples));
        shaper.processBaseRate(dryWrite, dryWrite, numSamples, drive);

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            outputBuffer.addFrom(channel, startSample, dry, 0, 0, numSamples);

        if (!stillRinging)
            isRinging = false;
    }

    OversampledWaveShaper shaper;
    juce::AudioBuffer<float> dry;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> driveSmoothed;
    Params params;
    double sampleRate = 48000.0;
    double phase = 0.0;
    std::int64_t elapsedSamples = 0;
    float currentVelocity = 1.0f;
    bool prepared = false;
    bool isRinging = false;
};

} // namespace dlz::drum
