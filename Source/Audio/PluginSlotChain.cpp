#include "PluginSlotChain.h"

namespace resonance::audio
{
PluginSlotChain::PluginSlotChain(int slotCount)
    : slotCount_(juce::jmax(1, slotCount))
{
    auto initial = std::make_shared<Runtime>();
    initial->slots.resize(static_cast<size_t>(slotCount_));
    messageRuntime = initial;
    activeRuntime.store(initial);
}

void PluginSlotChain::publishRuntime(std::shared_ptr<Runtime> next)
{
    messageRuntime = next;
    activeRuntime.store(std::move(next), std::memory_order_release);
}

void PluginSlotChain::rebuildProcessingOrder(Runtime& rt) const
{
    rt.processingOrder.clear();
    for (int i = 0; i < static_cast<int>(rt.slots.size()); ++i)
    {
        if (rt.slots[static_cast<size_t>(i)] != nullptr)
            rt.processingOrder.push_back(i);
    }
}

bool PluginSlotChain::slotAcceptsMidi(const Runtime& rt, int slotIndex) const
{
    if (slotIndex < 0 || slotIndex >= static_cast<int>(rt.slots.size()))
        return false;
    const auto& slot = rt.slots[static_cast<size_t>(slotIndex)];
    return slot != nullptr && slot->acceptsMidi();
}

void PluginSlotChain::prepare(double sampleRate, int blockSize)
{
    sampleRateHz = sampleRate;
    maxBlockSize = juce::jmax(1, blockSize);
    scratch.setSize(2, maxBlockSize);

    const juce::ScopedLock sl(writeLock);
    for (auto& slot : messageRuntime->slots)
    {
        if (slot != nullptr)
        {
            slot->setPlayConfigDetails(2, 2, sampleRateHz, maxBlockSize);
            slot->prepareToPlay(sampleRateHz, maxBlockSize);
        }
    }
}

void PluginSlotChain::releaseResources()
{
    const juce::ScopedLock sl(writeLock);
    for (auto& slot : messageRuntime->slots)
    {
        if (slot != nullptr)
            slot->releaseResources();
    }
}

std::vector<PluginSlotChain::SlotState> PluginSlotChain::getSlotStates() const
{
    std::vector<SlotState> out;
    const auto rt = activeRuntime.load(std::memory_order_acquire);
    if (rt == nullptr)
        return out;

    out.resize(rt->slots.size());
    for (int i = 0; i < static_cast<int>(rt->slots.size()); ++i)
    {
        if (rt->slots[static_cast<size_t>(i)] != nullptr)
        {
            out[static_cast<size_t>(i)].loaded = true;
            out[static_cast<size_t>(i)].name = rt->slots[static_cast<size_t>(i)]->getName();
            out[static_cast<size_t>(i)].isInstrument = rt->slots[static_cast<size_t>(i)]->acceptsMidi();
        }
        else
        {
            out[static_cast<size_t>(i)].name = "Empty";
        }
    }

    return out;
}

void PluginSlotChain::setProcessorInSlot(int slotIndex, std::unique_ptr<juce::AudioProcessor> instance)
{
    if (slotIndex < 0 || slotIndex >= slotCount_)
        return;

    const juce::ScopedLock sl(writeLock);

    auto next = std::make_shared<Runtime>();
    next->slots = std::move(messageRuntime->slots);
    if (next->slots.size() < static_cast<size_t>(slotCount_))
        next->slots.resize(static_cast<size_t>(slotCount_));

    if (next->slots[static_cast<size_t>(slotIndex)] != nullptr)
        next->slots[static_cast<size_t>(slotIndex)]->releaseResources();

    next->slots[static_cast<size_t>(slotIndex)] = std::move(instance);

    if (next->slots[static_cast<size_t>(slotIndex)] != nullptr)
    {
        auto& plugin = *next->slots[static_cast<size_t>(slotIndex)];
        plugin.setPlayConfigDetails(2, 2, sampleRateHz, maxBlockSize);
        plugin.prepareToPlay(sampleRateHz, maxBlockSize);
    }

    rebuildProcessingOrder(*next);
    publishRuntime(std::move(next));
}

juce::AudioProcessor* PluginSlotChain::getProcessorInSlot(int slotIndex) noexcept
{
    if (slotIndex < 0 || slotIndex >= slotCount_)
        return nullptr;

    const juce::ScopedLock sl(writeLock);
    return messageRuntime->slots[static_cast<size_t>(slotIndex)].get();
}

void PluginSlotChain::process(float* const* outputChannelData,
                              int numOutputChannels,
                              int numSamples,
                              juce::MidiBuffer& midi) noexcept
{
    if (outputChannelData == nullptr || numOutputChannels <= 0 || numSamples <= 0)
        return;

    const auto rt = activeRuntime.load(std::memory_order_acquire);
    if (rt == nullptr)
        return;

    juce::AudioBuffer<float> output;
    output.setDataToReferTo(outputChannelData, numOutputChannels, numSamples);
    output.clear();

    const int channels = juce::jmin(2, numOutputChannels);
    scratch.setSize(channels, numSamples, false, false, true);

    for (const int slotIndex : rt->processingOrder)
    {
        auto& plugin = rt->slots[static_cast<size_t>(slotIndex)];
        if (plugin == nullptr)
            continue;

        if (slotAcceptsMidi(*rt, slotIndex))
        {
            scratch.clear();
            plugin->processBlock(scratch, midi);

            for (int ch = 0; ch < channels; ++ch)
                output.addFrom(ch, 0, scratch, ch, 0, numSamples);
        }
        else
        {
            plugin->processBlock(output, emptyMidi);
        }
    }
}

void PluginSlotChain::processInsertsOnBuffer(juce::AudioBuffer<float>& buffer) noexcept
{
    const auto rt = activeRuntime.load(std::memory_order_acquire);
    if (rt == nullptr)
        return;

    for (const int slotIndex : rt->processingOrder)
    {
        auto& plugin = rt->slots[static_cast<size_t>(slotIndex)];
        if (plugin == nullptr || slotAcceptsMidi(*rt, slotIndex))
            continue;

        plugin->processBlock(buffer, emptyMidi);
    }
}
} // namespace resonance::audio
