#pragma once

#include <JuceHeader.h>
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

    int getSlotCount() const noexcept { return static_cast<int>(slots.size()); }

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
    void rebuildProcessingOrder();
    bool slotAcceptsMidi(int slotIndex) const;

    double sampleRateHz = 44100.0;
    int maxBlockSize = 512;
    mutable juce::CriticalSection lock;
    std::vector<std::unique_ptr<juce::AudioProcessor>> slots;
    std::vector<int> processingOrder;
    juce::AudioBuffer<float> scratch;
    juce::MidiBuffer emptyMidi;
};
} // namespace resonance::audio
