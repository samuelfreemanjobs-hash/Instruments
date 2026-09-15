#pragma once

#include <JuceHeader.h>
#include "PluginSlotChain.h"
#include "MixConsole.h"

namespace vmpc::audio
{
struct PluginSlotLocation
{
    enum class Bus
    {
        Master,
        Channel
    };

    Bus bus = Bus::Master;
    int channelIndex = 0;
    int slotIndex = 0;

    bool isValidForMaster() const noexcept { return bus == Bus::Master && slotIndex >= 0 && slotIndex < PluginSlotChain::kMasterSlots; }

    bool isValidForChannel() const noexcept
    {
        return bus == Bus::Channel && channelIndex >= 0 && channelIndex < MixConsole::kNumChannels && slotIndex >= 0
               && slotIndex < PluginSlotChain::kChannelInsertSlots;
    }

    uint32_t editorKey() const noexcept
    {
        if (bus == Bus::Master)
            return static_cast<uint32_t>(slotIndex);
        return 1000u + static_cast<uint32_t>(channelIndex) * 10u + static_cast<uint32_t>(slotIndex);
    }
};

inline bool operator==(const PluginSlotLocation& a, const PluginSlotLocation& b)
{
    return a.bus == b.bus && a.channelIndex == b.channelIndex && a.slotIndex == b.slotIndex;
}
} // namespace vmpc::audio
