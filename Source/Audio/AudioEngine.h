#pragma once

#include <JuceHeader.h>
#include "Model/AppMode.h"
#include "Model/Sequencer/SequencerCore.h"
#include "Model/Electribe/ElectribeSequencer.h"
#include "Model/Electribe/ElectribeSong.h"
#include <atomic>

namespace vmpc::audio
{
class MixConsole;

class AudioEngine : public juce::AudioIODeviceCallback
{
public:
    AudioEngine();

    void bindElectribeSong(model::ElectribeSong* song) noexcept;

    void setMixConsole(MixConsole* console) noexcept { mixConsole = console; }

    double getSampleRate() const noexcept { return deviceSampleRate.load(); }
    int getBlockSize() const noexcept { return deviceBlockSize.load(); }

    void setAppMode(model::AppMode mode) noexcept { appMode.store(static_cast<int>(mode)); }

    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                          int numInputChannels,
                                          float* const* outputChannelData,
                                          int numOutputChannels,
                                          int numSamples,
                                          const juce::AudioIODeviceCallbackContext& context) override;

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    model::SequencerCore& getSequencer() noexcept { return sequencer; }
    model::ElectribeSequencer& getElectribeSequencer() noexcept { return electribeSequencer; }

    float getPeakL() const noexcept { return peakL.load(); }
    float getPeakR() const noexcept { return peakR.load(); }

    int getPlayingStepForUi() const noexcept { return playingStepForUi.load(); }

private:
    model::SequencerCore sequencer;
    model::ElectribeSequencer electribeSequencer;
    MixConsole* mixConsole = nullptr;
    juce::MidiBuffer midiBuffer;
    std::atomic<double> deviceSampleRate { 44100.0 };
    std::atomic<int> deviceBlockSize { 512 };
    std::atomic<int> appMode { static_cast<int>(model::AppMode::Electribe) };
    std::atomic<int> playingStepForUi { 0 };
    std::atomic<float> peakL { 0.0f };
    std::atomic<float> peakR { 0.0f };
};
} // namespace vmpc::audio
