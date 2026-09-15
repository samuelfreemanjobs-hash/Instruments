#include "AudioEngine.h"

namespace vmpc::audio
{
AudioEngine::AudioEngine() = default;

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    if (device != nullptr)
        sequencer.prepare(device->getCurrentSampleRate());
}

void AudioEngine::audioDeviceStopped() {}

void AudioEngine::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                                     int numInputChannels,
                                                     float* const* outputChannelData,
                                                     int numOutputChannels,
                                                     int numSamples,
                                                     const juce::AudioIODeviceCallbackContext&)
{
    juce::ignoreUnused(inputChannelData, numInputChannels);

    midiBuffer.clear();
    sequencer.processBlock(numSamples, midiBuffer);

    float maxL = 0.0f;
    float maxR = 0.0f;

    for (int ch = 0; ch < numOutputChannels; ++ch)
    {
        if (outputChannelData[ch] == nullptr)
            continue;

        juce::FloatVectorOperations::clear(outputChannelData[ch], numSamples);

        // Phase 4: render sampler from midiBuffer into output buffers.
        for (int i = 0; i < numSamples; ++i)
        {
            const float v = outputChannelData[ch][i];
            if (ch == 0)
                maxL = juce::jmax(maxL, std::abs(v));
            else if (ch == 1)
                maxR = juce::jmax(maxR, std::abs(v));
        }
    }

    peakL.store(maxL);
    peakR.store(maxR);
}
} // namespace vmpc::audio
