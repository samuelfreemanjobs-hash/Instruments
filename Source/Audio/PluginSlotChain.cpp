#include "PluginSlotChain.h"

namespace resonance::audio
{
PluginSlotChain::PluginSlotChain(int slotCount)
{
    slots.resize(static_cast<size_t>(juce::jmax(1, slotCount)));
    processingOrder.reserve(slots.size());
}

void PluginSlotChain::prepare(double sampleRate, int blockSize)
{
    sampleRateHz = sampleRate;
    maxBlockSize = juce::jmax(1, blockSize);
    scratch.setSize(2, maxBlockSize);

    const juce::ScopedLock sl(lock);
    for (auto& slot : slots)
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
    const juce::ScopedLock sl(lock);
    for (auto& slot : slots)
    {
        if (slot != nullptr)
            slot->releaseResources();
    }
}

std::vector<PluginSlotChain::SlotState> PluginSlotChain::getSlotStates() const
{
    std::vector<SlotState> out;
    out.resize(slots.size());

    const juce::ScopedLock sl(lock);
    for (int i = 0; i < static_cast<int>(slots.size()); ++i)
    {
        if (slots[static_cast<size_t>(i)] != nullptr)
        {
            out[static_cast<size_t>(i)].loaded = true;
            out[static_cast<size_t>(i)].name = slots[static_cast<size_t>(i)]->getName();
            out[static_cast<size_t>(i)].isInstrument = slots[static_cast<size_t>(i)]->acceptsMidi();
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
    if (slotIndex < 0 || slotIndex >= static_cast<int>(slots.size()))
        return;

    const juce::ScopedLock sl(lock);

    if (slots[static_cast<size_t>(slotIndex)] != nullptr)
        slots[static_cast<size_t>(slotIndex)]->releaseResources();

    slots[static_cast<size_t>(slotIndex)] = std::move(instance);

    if (slots[static_cast<size_t>(slotIndex)] != nullptr)
    {
        auto& plugin = *slots[static_cast<size_t>(slotIndex)];
        plugin.setPlayConfigDetails(2, 2, sampleRateHz, maxBlockSize);
        plugin.prepareToPlay(sampleRateHz, maxBlockSize);
    }

    rebuildProcessingOrder();
}

juce::AudioProcessor* PluginSlotChain::getProcessorInSlot(int slotIndex) noexcept
{
    if (slotIndex < 0 || slotIndex >= static_cast<int>(slots.size()))
        return nullptr;
    return slots[static_cast<size_t>(slotIndex)].get();
}

void PluginSlotChain::rebuildProcessingOrder()
{
    processingOrder.clear();
    for (int i = 0; i < static_cast<int>(slots.size()); ++i)
    {
        if (slots[static_cast<size_t>(i)] != nullptr)
            processingOrder.push_back(i);
    }
}

bool PluginSlotChain::slotAcceptsMidi(int slotIndex) const
{
    const auto& slot = slots[static_cast<size_t>(slotIndex)];
    return slot != nullptr && slot->acceptsMidi();
}

void PluginSlotChain::process(float* const* outputChannelData,
                              int numOutputChannels,
                              int numSamples,
                              juce::MidiBuffer& midi) noexcept
{
    if (outputChannelData == nullptr || numOutputChannels <= 0 || numSamples <= 0)
        return;

    juce::AudioBuffer<float> output;
    output.setDataToReferTo(outputChannelData, numOutputChannels, numSamples);
    output.clear();

    const juce::ScopedTryLock tryLock(lock);
    if (!tryLock.isLocked())
        return;

    const int channels = juce::jmin(2, numOutputChannels);
    scratch.setSize(channels, numSamples, false, false, true);

    for (const int slotIndex : processingOrder)
    {
        auto& plugin = slots[static_cast<size_t>(slotIndex)];
        if (plugin == nullptr)
            continue;

        if (slotAcceptsMidi(slotIndex))
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
    const juce::ScopedTryLock tryLock(lock);
    if (!tryLock.isLocked())
        return;

    for (const int slotIndex : processingOrder)
    {
        auto& plugin = slots[static_cast<size_t>(slotIndex)];
        if (plugin == nullptr || slotAcceptsMidi(slotIndex))
            continue;

        plugin->processBlock(buffer, emptyMidi);
    }
}
} // namespace resonance::audio
