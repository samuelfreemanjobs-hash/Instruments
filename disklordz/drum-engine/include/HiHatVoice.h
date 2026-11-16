#pragma once

#include "OversampledWaveShaper.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <array>
#include <cmath>

namespace dlz::drum {

class HiHatVoice {
  public:
    struct Params {
        float fundamentalHz = 317.0f;
        float decaySec = 0.045f;
        float drive = 1.4f;
        float hpHz = 7000.0f;
        float bpHz = 10500.0f;
        bool openHat = false;
    };

    void prepare(const juce::dsp::ProcessSpec &spec) {
        sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 48000.0;
        shaper.prepare(sampleRate, static_cast<int>(spec.maximumBlockSize));
        const auto osRate = shaper.oversampledRate();
        const auto osBlock = static_cast<juce::uint32>(shaper.capacity() * juce::jmax(1, shaper.oversamplingFactor()));
        juce::dsp::ProcessSpec filterSpec{osRate, osBlock, 1};
        highpass.prepare(filterSpec);
        bandpass.prepare(filterSpec);
        highpass.setType(juce::dsp::StateVariableTPTFilterType::highpass);
        bandpass.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
        highpass.setResonance(0.707f);
        bandpass.setResonance(0.9f);
        prepared = true;
    }

    void trigger(const Params &newParams, float velocity = 1.0f) {
        params = newParams;
        params.fundamentalHz = juce::jlimit(80.0f, 800.0f, params.fundamentalHz);
        if (params.openHat)
            params.decaySec = juce::jmax(params.decaySec, 0.25f);
        params.decaySec = juce::jmax(0.01f, params.decaySec);
        params.drive = juce::jmax(1.0f, params.drive);
        currentVelocity = juce::jlimit(0.0f, 1.0f, velocity);
        phases.fill(0.0);
        elapsedSamples = 0;
        highpass.setCutoffFrequency(juce::jlimit(1000.0f, 16000.0f, params.hpHz));
        bandpass.setCutoffFrequency(juce::jlimit(2000.0f, 18000.0f, params.bpHz));
        highpass.reset();
        bandpass.reset();
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
    int oversamplingFactor() const noexcept { return shaper.oversamplingFactor(); }

  private:
    void renderChunk(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
        const double osRate = shaper.oversampledRate();
        const std::int64_t chunkStart = elapsedSamples;
        bool stillRinging = false;

        shaper.processGenerated(numSamples, params.drive, [&](float *upsampled, int upsampledCount) {
            for (int i = 0; i < upsampledCount; ++i) {
                const double timeSec =
                    (static_cast<double>(chunkStart) +
                     (static_cast<double>(i) / static_cast<double>(juce::jmax(1, shaper.oversamplingFactor())))) /
                    sampleRate;
                const double env = std::exp(-timeSec / static_cast<double>(params.decaySec));
                if (env < 1.0e-5) {
                    upsampled[i] = 0.0f;
                    continue;
                }

                stillRinging = true;
                float sum = 0.0f;
                for (size_t osc = 0; osc < ratios.size(); ++osc) {
                    phases[osc] += (static_cast<double>(params.fundamentalHz) * ratios[osc]) / osRate;
                    phases[osc] -= std::floor(phases[osc]);
                    sum += phases[osc] < 0.5 ? 1.0f : -1.0f;
                }
                float sample = (sum / static_cast<float>(ratios.size())) * static_cast<float>(env) * currentVelocity;
                sample = highpass.processSample(0, sample);
                sample = bandpass.processSample(0, sample);
                upsampled[i] = sample;
            }
        });

        elapsedSamples += numSamples;
        const float *wet = shaper.wetSamples();
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            outputBuffer.addFrom(channel, startSample, wet, numSamples);
        if (!stillRinging)
            isRinging = false;
    }

    static constexpr std::array<double, 6> ratios{1.0, 1.483, 1.932, 2.546, 2.630, 3.897};

    OversampledWaveShaper shaper;
    juce::dsp::StateVariableTPTFilter<float> highpass;
    juce::dsp::StateVariableTPTFilter<float> bandpass;
    Params params;
    std::array<double, 6> phases{};
    double sampleRate = 48000.0;
    std::int64_t elapsedSamples = 0;
    float currentVelocity = 1.0f;
    bool prepared = false;
    bool isRinging = false;
};

} // namespace dlz::drum
