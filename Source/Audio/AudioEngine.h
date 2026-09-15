#pragma once

#include <JuceHeader.h>
#include "Model/Sequencer/SequencerCore.h"

namespace vmpc::audio
{
/**
 * Real-time I/O callback. Phase 4 will add sampler + FX here.
 * UI reads peak meters via atomics only.
 */
class AudioEngine : public juce::AudioIODeviceCallback
{
public:
    AudioEngine();

    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                          int numInputChannels,
                                          float* const* outputChannelData,
                                          int numOutputChannels,
                                          int numSamples,
                                          const juce::AudioIODeviceCallbackContext& context) override;

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    model::SequencerCore& getSequencer() noexcept { return sequencer; }

    float getPeakL() const noexcept { return peakL.load(); }
    float getPeakR() const noexcept { return peakR.load(); }

private:
    model::SequencerCore sequencer;
    juce::MidiBuffer midiBuffer;
    std::atomic<float> peakL { 0.0f };
    std::atomic<float> peakR { 0.0f };
};
} // namespace vmpc::audio
