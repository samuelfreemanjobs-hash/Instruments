#pragma once

#include "OversampledWaveShaper.h"

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <cmath>
#include <cstdint>

namespace dlz::drum {

class SnareVoice {
  public:
    struct Params {
        float toneHz = 185.0f;
        float overtoneRatio = 1.8f;
        float noiseMix = 0.45f;
        float ampDecaySec = 0.28f;
        float drive = 1.8f;
        float noiseCutoffHz = 2200.0f;
    };

    void prepare(const juce::dsp::ProcessSpec &spec) {
        sampleRate = spec.sampleRate > 0.0 ? spec.sampleRate : 48000.0;
        shaper.prepare(sampleRate, static_cast<int>(spec.maximumBlockSize));
        dry.setSize(1, shaper.capacity(), false, false, true);
        juce::dsp::ProcessSpec filterSpec{sampleRate, static_cast<juce::uint32>(shaper.capacity()), 1};
        noiseFilter.prepare(filterSpec);
        noiseFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
        noiseFilter.setResonance(0.7f);
        prepared = true;
    }

    void trigger(const Params &newParams, float velocity = 1.0f) {
        params = newParams;
        params.toneHz = juce::jlimit(80.0f, 500.0f, params.toneHz);
        params.overtoneRatio = juce::jlimit(1.2f, 3.0f, params.overtoneRatio);
        params.noiseMix = juce::jlimit(0.0f, 1.0f, params.noiseMix);
        params.ampDecaySec = juce::jmax(0.02f, params.ampDecaySec);
        params.drive = juce::jmax(1.0f, params.drive);
        currentVelocity = juce::jlimit(0.0f, 1.0f, velocity);
        bodyPhase = 0.0;
        overtonePhase = 0.0;
        elapsedSamples = 0;
        noiseState = 0xA341316Cu;
        pink = 0.0f;
        noiseFilter.setCutoffFrequency(juce::jlimit(200.0f, 8000.0f, params.noiseCutoffHz));
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
    int scratchCapacity() const noexcept { return shaper.capacity(); }

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
        const float bodyMix = 1.0f - params.noiseMix;

        for (int i = 0; i < numSamples; ++i) {
            const double timeSec = static_cast<double>(elapsedSamples) / sampleRate;
            const double ampEnv = std::exp(-timeSec / static_cast<double>(params.ampDecaySec));
            ++elapsedSamples;
            if (ampEnv < 1.0e-5) {
                dryWrite[i] = 0.0f;
                continue;
            }

            stillRinging = true;
            bodyPhase += static_cast<double>(params.toneHz) / sampleRate;
            overtonePhase += static_cast<double>(params.toneHz * params.overtoneRatio) / sampleRate;
            bodyPhase -= std::floor(bodyPhase);
            overtonePhase -= std::floor(overtonePhase);

            const float body = static_cast<float>(std::sin(bodyPhase * juce::MathConstants<double>::twoPi));
            const float overtone = triangle(overtonePhase);
            const float white = nextWhite();
            pink += 0.05f * (white - pink);
            const float colored = noiseFilter.processSample(0, white * 0.7f + pink * 0.3f);
            const float mixed = (body * 0.75f + overtone * 0.25f) * bodyMix + colored * params.noiseMix;
            dryWrite[i] = mixed * static_cast<float>(ampEnv) * currentVelocity;
        }

        shaper.processBaseRate(dryWrite, dryWrite, numSamples, params.drive);
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            outputBuffer.addFrom(channel, startSample, dry, 0, 0, numSamples);

        if (!stillRinging)
            isRinging = false;
    }

    static float triangle(double phase) noexcept { return static_cast<float>(1.0 - 4.0 * std::abs(phase - 0.5)); }

    OversampledWaveShaper shaper;
    juce::dsp::StateVariableTPTFilter<float> noiseFilter;
    juce::AudioBuffer<float> dry;
    Params params;
    double sampleRate = 48000.0;
    double bodyPhase = 0.0;
    double overtonePhase = 0.0;
    std::int64_t elapsedSamples = 0;
    std::uint32_t noiseState = 0xA341316Cu;
    float pink = 0.0f;
    float currentVelocity = 1.0f;
    bool prepared = false;
    bool isRinging = false;
};

} // namespace dlz::drum
