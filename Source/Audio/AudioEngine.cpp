#include "AudioEngine.h"
#include "MixConsole.h"

namespace resonance::audio
{
AudioEngine::AudioEngine() = default;

void AudioEngine::bindElectribeSong(model::ElectribeSong* song) noexcept
{
    electribeSequencer.setSong(song);
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    if (device != nullptr)
    {
        const double sr = device->getCurrentSampleRate();
        const int block = device->getCurrentBufferSizeSamples();
        deviceSampleRate.store(sr);
        deviceBlockSize.store(block);
        sequencer.prepare(sr);
        electribeSequencer.prepare(sr);
        if (mixConsole != nullptr)
            mixConsole->prepare(sr, block);
    }
}

void AudioEngine::audioDeviceStopped()
{
    if (mixConsole != nullptr)
        mixConsole->releaseResources();
}

void AudioEngine::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                                     int numInputChannels,
                                                     float* const* outputChannelData,
                                                     int numOutputChannels,
                                                     int numSamples,
                                                     const juce::AudioIODeviceCallbackContext&)
{
    juce::ignoreUnused(inputChannelData, numInputChannels);

    midiBuffer.clear();

    const auto mode = static_cast<model::AppMode>(appMode.load());
    if (mode == model::AppMode::HybridMpc)
    {
        sequencer.processBlock(numSamples, midiBuffer);
        playingStepForUi.store(sequencer.getPlayingStepForUi());
    }
    else
    {
        electribeSequencer.processBlock(numSamples, midiBuffer);
        playingStepForUi.store(electribeSequencer.getPlayingStepForUi());
    }

    if (mixConsole != nullptr)
        mixConsole->process(outputChannelData, numOutputChannels, numSamples, midiBuffer);
    else
    {
        for (int ch = 0; ch < numOutputChannels; ++ch)
            if (outputChannelData[ch] != nullptr)
                juce::FloatVectorOperations::clear(outputChannelData[ch], numSamples);
    }

    float maxL = 0.0f;
    float maxR = 0.0f;

    for (int ch = 0; ch < numOutputChannels; ++ch)
    {
        if (outputChannelData[ch] == nullptr)
            continue;

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
} // namespace resonance::audio
