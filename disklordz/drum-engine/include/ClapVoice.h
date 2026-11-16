#pragma once

#include "OversampledWaveShaper.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <cmath>
#include <cstdint>

namespace dlz::drum {

class ClapVoice {
  public:
    struct Params {
        float spacingSec = 0.011f;
        float burstDecaySec = 0.008f;
        float tailDecaySec = 0.18f;
        float drive = 1.6f;
        float noiseCutoffHz = 1400.0f;
    };

    void prepare(const juce::dsp::ProcessSpec &spec) {
        sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 48000.0;
        shaper.prepare(sampleRate, static_cast<int>(spec.maximumBlockSize));
        dry.setSize(1, shaper.capacity(), false, false, true);
        juce::dsp::ProcessSpec filterSpec{sampleRate, static_cast<juce::uint32>(shaper.capacity()), 1};
        noiseFilter.prepare(filterSpec);
        noiseFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
        noiseFilter.setResonance(0.8f);
        prepared = true;
    }

    void trigger(const Params &newParams, float velocity = 1.0f) {
        params = newParams;
        params.spacingSec = juce::jlimit(0.008f, 0.015f, params.spacingSec);
        params.burstDecaySec = juce::jmax(0.002f, params.burstDecaySec);
        params.tailDecaySec = juce::jmax(0.04f, params.tailDecaySec);
        params.drive = juce::jmax(1.0f, params.drive);
        currentVelocity = juce::jlimit(0.0f, 1.0f, velocity);
        elapsedSamples = 0;
        noiseState = 0xC0FFEE01u;
        noiseFilter.setCutoffFrequency(juce::jlimit(400.0f, 6000.0f, params.noiseCutoffHz));
        noiseFilter.reset();
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

  private:
    float nextWhite() noexcept {
        noiseState ^= noiseState << 13;
        noiseState ^= noiseState >> 17;
        noiseState ^= noiseState << 5;
        return static_cast<float>(static_cast<std::int32_t>(noiseState)) * (1.0f / 2147483648.0f);
    }

    void renderChunk(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
        auto *dryWrite = dry.getWritePointer(0);
        bool stillRinging = false;

        for (int i = 0; i < numSamples; ++i) {
            const double timeSec = static_cast<double>(elapsedSamples) / sampleRate;
            ++elapsedSamples;

            double burst = 0.0;
            for (int tap = 0; tap < 4; ++tap) {
                const double start = static_cast<double>(tap) * static_cast<double>(params.spacingSec);
                const double age = timeSec - start;
                if (age >= 0.0 && age < 0.03)
                    burst += std::exp(-age / static_cast<double>(params.burstDecaySec));
            }
            const double tail = std::exp(-timeSec / static_cast<double>(params.tailDecaySec));
            const double env = juce::jmin(1.0, burst) * 0.85 + tail * 0.35;
            if (env < 1.0e-4) {
                dryWrite[i] = 0.0f;
                continue;
            }

            stillRinging = true;
            const float noise = noiseFilter.processSample(0, nextWhite());
            dryWrite[i] = noise * static_cast<float>(env) * currentVelocity;
        }

        shaper.processBaseRate(dryWrite, dryWrite, numSamples, params.drive);
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            outputBuffer.addFrom(channel, startSample, dry, 0, 0, numSamples);
        if (!stillRinging)
            isRinging = false;
    }

    OversampledWaveShaper shaper;
    juce::dsp::StateVariableTPTFilter<float> noiseFilter;
    juce::AudioBuffer<float> dry;
    Params params;
    double sampleRate = 48000.0;
    std::int64_t elapsedSamples = 0;
    std::uint32_t noiseState = 1;
    float currentVelocity = 1.0f;
    bool prepared = false;
    bool isRinging = false;
};

} // namespace dlz::drum
