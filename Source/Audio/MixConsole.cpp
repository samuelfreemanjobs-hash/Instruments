#include "MixConsole.h"

namespace vmpc::audio
{
MixConsole::MixConsole()
    : masterChain(PluginSlotChain::kMasterSlots)
    , channelChains { PluginSlotChain(PluginSlotChain::kChannelInsertSlots),
                      PluginSlotChain(PluginSlotChain::kChannelInsertSlots),
                      PluginSlotChain(PluginSlotChain::kChannelInsertSlots),
                      PluginSlotChain(PluginSlotChain::kChannelInsertSlots),
                      PluginSlotChain(PluginSlotChain::kChannelInsertSlots),
                      PluginSlotChain(PluginSlotChain::kChannelInsertSlots),
                      PluginSlotChain(PluginSlotChain::kChannelInsertSlots),
                      PluginSlotChain(PluginSlotChain::kChannelInsertSlots) }
{
    for (auto& f : channelFaders)
        f = 1.0f;
}

void MixConsole::prepare(double sampleRate, int blockSize)
{
    masterChain.prepare(sampleRate, blockSize);
    for (auto& chain : channelChains)
        chain.prepare(sampleRate, blockSize);
    workBuffer.setSize(2, blockSize);
}

void MixConsole::releaseResources()
{
    masterChain.releaseResources();
    for (auto& chain : channelChains)
        chain.releaseResources();
}

PluginSlotChain& MixConsole::getChannelInserts(int channelIndex) noexcept
{
    return channelChains[static_cast<size_t>(juce::jlimit(0, kNumChannels - 1, channelIndex))];
}

float MixConsole::getChannelFader(int channelIndex) const noexcept
{
    if (channelIndex < 0 || channelIndex >= kNumChannels)
        return 1.0f;
    return channelFaders[static_cast<size_t>(channelIndex)];
}

void MixConsole::setChannelFader(int channelIndex, float level01) noexcept
{
    if (channelIndex < 0 || channelIndex >= kNumChannels)
        return;
    channelFaders[static_cast<size_t>(channelIndex)] = juce::jlimit(0.0f, 1.0f, level01);
}

bool MixConsole::isChannelMuted(int channelIndex) const noexcept
{
    if (channelIndex < 0 || channelIndex >= kNumChannels)
        return false;
    return channelMuted[static_cast<size_t>(channelIndex)];
}

void MixConsole::setChannelMuted(int channelIndex, bool muted) noexcept
{
    if (channelIndex < 0 || channelIndex >= kNumChannels)
        return;
    channelMuted[static_cast<size_t>(channelIndex)] = muted;
}

void MixConsole::refreshChannelInsertFlags() noexcept
{
    for (int i = 0; i < kNumChannels; ++i)
    {
        bool any = false;
        const auto states = channelChains[static_cast<size_t>(i)].getSlotStates();
        for (const auto& state : states)
        {
            if (state.loaded)
            {
                any = true;
                break;
            }
        }
        channelInsertActive[static_cast<size_t>(i)].store(any);
    }
}

bool MixConsole::channelHasInserts(int channelIndex) const noexcept
{
    if (channelIndex < 0 || channelIndex >= kNumChannels)
        return false;
    return channelInsertActive[static_cast<size_t>(channelIndex)].load();
}

void MixConsole::process(float* const* outputChannelData,
                         int numOutputChannels,
                         int numSamples,
                         juce::MidiBuffer& midi) noexcept
{
    masterChain.process(outputChannelData, numOutputChannels, numSamples, midi);

    if (numOutputChannels <= 0 || numSamples <= 0)
        return;

    const int channels = juce::jmin(2, numOutputChannels);
    workBuffer.setSize(channels, numSamples, false, false, true);
    for (int ch = 0; ch < channels; ++ch)
        workBuffer.copyFrom(ch, 0, outputChannelData[ch], numSamples);

    for (int channelIndex = 0; channelIndex < kNumChannels; ++channelIndex)
    {
        if (isChannelMuted(channelIndex) || !channelHasInserts(channelIndex))
            continue;

        channelChains[static_cast<size_t>(channelIndex)].processInsertsOnBuffer(workBuffer);
    }

    for (int ch = 0; ch < channels; ++ch)
    {
        if (outputChannelData[ch] != nullptr)
            juce::FloatVectorOperations::copy(outputChannelData[ch], workBuffer.getReadPointer(ch), numSamples);
    }
}
} // namespace vmpc::audio
