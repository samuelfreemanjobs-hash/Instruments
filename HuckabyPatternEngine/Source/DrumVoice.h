#pragma once

#include "SampleTypes.h"
#include <JuceHeader.h>

namespace huckaby
{

/** One-shot linearly interpolated sample voice. */
class DrumVoice final
{
public:
    DrumVoice() = default;

    void trigger(std::shared_ptr<SampleData> sampleToPlay, float gain) noexcept
    {
        activeSample = std::move(sampleToPlay);
        playbackPosition = 0.0;
        velocityGain = gain;
        isPlaying = (activeSample != nullptr && activeSample->lengthInSamples > 1);
    }

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                         int startSample,
                         int numSamples,
                         double hostSampleRate) noexcept
    {
        if (!isPlaying || activeSample == nullptr)
            return;

        const int bufferLength = activeSample->lengthInSamples;
        const auto& srcBuffer = activeSample->buffer;
        const int numChannels = juce::jmin(outputBuffer.getNumChannels(), srcBuffer.getNumChannels());

        const double phaseIncrement = (hostSampleRate > 0.0 && activeSample->sampleRate > 0.0)
                                          ? (activeSample->sampleRate / hostSampleRate)
                                          : 1.0;

        for (int i = 0; i < numSamples; ++i)
        {
            const int currentIntPos = static_cast<int>(playbackPosition);
            if (currentIntPos >= bufferLength - 1)
            {
                isPlaying = false;
                activeSample.reset();
                break;
            }

            const double alpha = playbackPosition - static_cast<double>(currentIntPos);

            for (int ch = 0; ch < numChannels; ++ch)
            {
                const float* channelData = srcBuffer.getReadPointer(ch);
                const float sample1 = channelData[currentIntPos];
                const float sample2 = channelData[juce::jmin(currentIntPos + 1, bufferLength - 1)];
                const float interpolated = sample1 + static_cast<float>(alpha) * (sample2 - sample1);

                outputBuffer.addSample(ch, startSample + i, interpolated * velocityGain);
            }

            playbackPosition += phaseIncrement;
        }
    }

    bool isActive() const noexcept { return isPlaying; }

private:
    std::shared_ptr<SampleData> activeSample;
    double playbackPosition { 0.0 };
    float velocityGain { 1.0f };
    bool isPlaying { false };
};

} // namespace huckaby
