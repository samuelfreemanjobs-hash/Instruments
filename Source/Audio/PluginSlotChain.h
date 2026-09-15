#pragma once

#include <JuceHeader.h>
#include <array>
#include <memory>
#include <vector>

namespace vmpc::audio
{
/** Fixed insert rack processed on the audio thread (instruments then effects). */
class PluginSlotChain
{
public:
    static constexpr int kNumSlots = 8;

    PluginSlotChain();

    void prepare(double sampleRate, int blockSize);
    void releaseResources();

    void process(float* const* outputChannelData,
                 int numOutputChannels,
                 int numSamples,
                 juce::MidiBuffer& midi) noexcept;

    struct SlotState
    {
        juce::String name;
        bool loaded = false;
        bool isInstrument = false;
    };

    std::array<SlotState, kNumSlots> getSlotStates() const;

    /** Message thread only. Replaces plugin in slot (nullptr clears). */
    void setPluginInSlot(int slotIndex, std::unique_ptr<juce::AudioPluginInstance> instance);

    juce::AudioPluginInstance* getPluginInSlot(int slotIndex) noexcept;

private:
    void rebuildProcessingOrder();
    bool slotAcceptsMidi(int slotIndex) const;

    double sampleRateHz = 44100.0;
    int maxBlockSize = 512;
    mutable juce::CriticalSection lock;
    std::array<std::unique_ptr<juce::AudioPluginInstance>, kNumSlots> slots {};
    std::vector<int> processingOrder;
    juce::AudioBuffer<float> scratch;
    juce::MidiBuffer emptyMidi;
};
} // namespace vmpc::audio
