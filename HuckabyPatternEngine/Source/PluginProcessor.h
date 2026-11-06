#pragma once

#include "DrumVoice.h"
#include "TrackModel.h"
#include <JuceHeader.h>
#include <array>
#include <atomic>

namespace huckaby
{

class HuckabyAudioProcessor final : public juce::AudioProcessor
{
public:
    HuckabyAudioProcessor();
    ~HuckabyAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    using juce::AudioProcessor::processBlock;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }

    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& dest) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getApvts() noexcept { return apvts; }

    void setPatternBits(int trackIndex, uint64_t bits) noexcept;
    uint64_t getPatternBits(int trackIndex) const noexcept;

    void requestLoadSample(int trackIndex, int slotIndex, const juce::File& file);

    void resetTransportCounters();

private:
    struct TrackParams final
    {
        std::atomic<float>* length { nullptr };
        std::atomic<float>* mode { nullptr };
        std::atomic<float>* staticBank { nullptr };
        std::atomic<float>* modulo { nullptr };
    };

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void refreshParameterHandles();

    void updateTransportAndTempo();
    void reconcilePlaybackStartStopEdge();

    void processIncomingMidi(const juce::MidiBuffer& midi) noexcept;

    void onSequencerTick() noexcept;
    void fireTrackIfTriggered(int trackIndex, int64_t globalTick) noexcept;

    void renderVoices(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) noexcept;
    int findFreeVoiceIndex() noexcept;

    void pullPatternBitsIntoTracks() noexcept;

    int getTrackLengthSteps(int trackIndex) const noexcept;
    SampleSelectionMode getTrackMode(int trackIndex) const noexcept;
    int getStaticBankIndex(int trackIndex) const noexcept;
    int getModuloDivisor(int trackIndex) const noexcept;

    bool isEffectivelyPlaying() const noexcept;

    juce::AudioProcessorValueTreeState apvts;

    std::array<TrackParams, kNumTracks> trackParams {};

    std::array<TrackState, kNumTracks> tracks {};
    std::array<std::atomic<uint64_t>, kNumTracks> patternBits {};

    std::array<DrumVoice, kMaxVoices> voices {};

    double hostSampleRate { 44100.0 };
    double currentBpm { 120.0 };
    double samplesPerStep { 0.0 };
    double sampleCounter { 0.0 };

    std::atomic<float>* internalBpmParam { nullptr };
    std::atomic<float>* internalPlayParam { nullptr };
    std::atomic<float>* useInternalTransportParam { nullptr };
    std::atomic<float>* outputGainDbParam { nullptr };

    bool hostIsPlaying { false };
    bool lastEffectivePlaying { false };

    int64_t globalTickIndex { 0 };

    juce::ThreadPool loadPool { 1 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HuckabyAudioProcessor)
};

} // namespace huckaby
