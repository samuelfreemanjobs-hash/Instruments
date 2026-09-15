#pragma once

#include <JuceHeader.h>
#include "Audio/PluginHostService.h"

namespace vmpc::model
{
inline const juce::Identifier mixingRootId { "Mixing" };
inline const juce::Identifier masterRackId { "MasterRack" };
inline const juce::Identifier channelId { "Channel" };
inline const juce::Identifier slotId { "Slot" };

void writeMixingToProject(juce::ValueTree& projectRoot, vmpc::audio::PluginHostService& host);
void readMixingFromProject(const juce::ValueTree& projectRoot, vmpc::audio::PluginHostService& host);
} // namespace vmpc::model
