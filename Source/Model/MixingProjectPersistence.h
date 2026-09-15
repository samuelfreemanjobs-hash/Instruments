#pragma once

#include <JuceHeader.h>
#include "Audio/PluginHostService.h"

namespace resonance::model
{
inline const juce::Identifier mixingRootId { "Mixing" };
inline const juce::Identifier masterRackId { "MasterRack" };
inline const juce::Identifier channelId { "Channel" };
inline const juce::Identifier slotId { "Slot" };
inline const juce::Identifier vibeHistoryId { "VibeHistory" };
inline const juce::Identifier vibeBriefId { "Brief" };
inline const juce::Identifier savedVibesId { "SavedVibes" };
inline const juce::Identifier savedVibeId { "Vibe" };

juce::ValueTree captureMixingSubtree(resonance::audio::PluginHostService& host);
void applyMixingSubtree(const juce::ValueTree& mixing, resonance::audio::PluginHostService& host);

void writeMixingToProject(juce::ValueTree& projectRoot, resonance::audio::PluginHostService& host);
void readMixingFromProject(const juce::ValueTree& projectRoot, resonance::audio::PluginHostService& host);
} // namespace resonance::model
