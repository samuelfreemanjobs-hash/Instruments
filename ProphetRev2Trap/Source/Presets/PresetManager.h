#pragma once

#include <juce_core/juce_core.h>

namespace prophetrev2::presets
{

class PresetManager
{
public:
    /** v0.1 mix roles + User bank. */
    static juce::StringArray getCategoryOrder();
    static juce::StringArray getPresetNamesForCategory (const juce::String& category);
    static int getGlobalIndexForCategoryPreset (const juce::String& category, int presetIndexInCategory);
    static void getCategoryAndLocalIndex (int globalIndex, juce::String& categoryOut, int& localIndexOut);
    static int getNumFactoryPresets() noexcept;
    static bool isUserCategory (const juce::String& category) noexcept;
};

} // namespace prophetrev2::presets
