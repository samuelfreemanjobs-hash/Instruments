#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <memory>
#include <vector>

namespace resonance::audio
{
/** Insert chain processed on the audio thread (instruments then effects). */
class PluginSlotChain
{
public:
    static constexpr int kMasterSlots = 8;
    static constexpr int kChannelInsertSlots = 2;

    explicit PluginSlotChain(int slotCount = kMasterSlots);

    int getSlotCount() const noexcept { return slotCount_; }

    void prepare(double sampleRate, int blockSize);
    void releaseResources();

    void process(float* const* outputChannelData,
                 int numOutputChannels,
                 int numSamples,
                 juce::MidiBuffer& midi) noexcept;

    /** Process FX-only (no MIDI instruments). */
    void processInsertsOnBuffer(juce::AudioBuffer<float>& buffer) noexcept;

    struct SlotState
    {
        juce::String name;
        bool loaded = false;
        bool isInstrument = false;
    };

    std::vector<SlotState> getSlotStates() const;

    void setProcessorInSlot(int slotIndex, std::unique_ptr<juce::AudioProcessor> processor);
    juce::AudioProcessor* getProcessorInSlot(int slotIndex) noexcept;

private:
    struct Runtime
    {
        std::vector<std::unique_ptr<juce::AudioProcessor>> slots;
        std::vector<int> processingOrder;
    };

    void publishRuntime(std::shared_ptr<Runtime> next);
    void rebuildProcessingOrder(Runtime& rt) const;
    bool slotAcceptsMidi(const Runtime& rt, int slotIndex) const;

    int slotCount_ = kMasterSlots;
    double sampleRateHz = 44100.0;
    int maxBlockSize = 512;
    mutable juce::CriticalSection writeLock;
    std::shared_ptr<Runtime> messageRuntime;
    std::atomic<std::shared_ptr<Runtime>> activeRuntime;
    juce::AudioBuffer<float> scratch;
    juce::MidiBuffer emptyMidi;
};
} // namespace resonance::audio
