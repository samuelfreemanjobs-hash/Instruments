#pragma once

#include "PluginSlotChain.h"
#include <array>
#include <atomic>

namespace vmpc::audio
{
/** Master rack + per-channel insert strips (post-mix serial FX until multitrack buses land). */
class MixConsole
{
public:
    static constexpr int kNumChannels = 8;

    MixConsole();

    void prepare(double sampleRate, int blockSize);
    void releaseResources();

    void process(float* const* outputChannelData,
                 int numOutputChannels,
                 int numSamples,
                 juce::MidiBuffer& midi) noexcept;

    PluginSlotChain& getMasterChain() noexcept { return masterChain; }
    PluginSlotChain& getChannelInserts(int channelIndex) noexcept;

    float getChannelFader(int channelIndex) const noexcept;
    void setChannelFader(int channelIndex, float level01) noexcept;

    bool isChannelMuted(int channelIndex) const noexcept;
    void setChannelMuted(int channelIndex, bool muted) noexcept;

    void refreshChannelInsertFlags() noexcept;

private:
    bool channelHasInserts(int channelIndex) const noexcept;

    PluginSlotChain masterChain;
    std::array<PluginSlotChain, kNumChannels> channelChains;
    std::array<float, kNumChannels> channelFaders {};
    std::array<bool, kNumChannels> channelMuted {};
    std::array<std::atomic<bool>, kNumChannels> channelInsertActive {};
    juce::AudioBuffer<float> workBuffer;
};
} // namespace vmpc::audio
