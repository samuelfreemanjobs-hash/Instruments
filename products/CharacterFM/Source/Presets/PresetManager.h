#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace characterfm::presets
{

/** Serializes APVTS state to versioned JSON (canonical preset v1 subset). */
juce::String exportPresetJson (const juce::ValueTree& state, const juce::String& presetName);
bool importPresetJson (const juce::String& json, juce::ValueTree& targetState);

} // namespace characterfm::presets
