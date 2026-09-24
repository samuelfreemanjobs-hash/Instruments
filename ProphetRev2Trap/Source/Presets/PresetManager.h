#pragma once

#include <juce_core/juce_core.h>

namespace prophetrev2::presets
{

/** Factory preset navigation by category (UI preset manager). */
class PresetManager
{
public:
    static juce::StringArray getCategoryOrder();
    static juce::StringArray getPresetNamesForCategory (const juce::String& category);
    static int getGlobalIndexForCategoryPreset (const juce::String& category, int presetIndexInCategory);
    static void getCategoryAndLocalIndex (int globalIndex, juce::String& categoryOut, int& localIndexOut);
    static int getNumPresets() noexcept;
};

} // namespace prophetrev2::presets
