#include "Presets/PresetManager.h"

#include "Presets/FactoryPresets.h"

namespace prophetrev2::presets
{

namespace
{
juce::String toJuce (std::string_view sv)
{
    return juce::String (sv.data(), static_cast<int> (sv.size()));
}
} // namespace

juce::StringArray PresetManager::getCategoryOrder()
{
    // Fixed mix-role order (see docs/PRESET_CATEGORIES.md). Skip empty categories.
    static const char* kRoleOrder[] = {
        "Bass", "Synth", "Lead", "Pad", "Pluck", "Keys", "Synth FX",
    };

    juce::StringArray present;
    for (const auto& preset : getFactoryPresets())
    {
        const juce::String cat = toJuce (preset.category);
        if (! present.contains (cat))
            present.add (cat);
    }

    juce::StringArray ordered;
    for (const char* role : kRoleOrder)
    {
        const juce::String cat (role);
        if (present.contains (cat))
            ordered.add (cat);
    }
    for (const auto& cat : present)
        if (! ordered.contains (cat))
            ordered.add (cat);
    return ordered;
}

juce::StringArray PresetManager::getPresetNamesForCategory (const juce::String& category)
{
    juce::StringArray names;
    for (const auto& preset : getFactoryPresets())
        if (category == toJuce (preset.category))
            names.add (toJuce (preset.name));
    return names;
}

int PresetManager::getGlobalIndexForCategoryPreset (const juce::String& category, int presetIndexInCategory)
{
    int local = 0;
    for (std::size_t i = 0; i < getFactoryPresets().size(); ++i)
    {
        if (category != toJuce (getFactoryPresets()[i].category))
            continue;
        if (local == presetIndexInCategory)
            return static_cast<int> (i);
        ++local;
    }
    return 0;
}

void PresetManager::getCategoryAndLocalIndex (int globalIndex, juce::String& categoryOut, int& localIndexOut)
{
    const auto& all = getFactoryPresets();
    if (globalIndex < 0 || globalIndex >= static_cast<int> (all.size()))
    {
        categoryOut = getCategoryOrder()[0];
        localIndexOut = 0;
        return;
    }

    categoryOut = toJuce (all[static_cast<std::size_t> (globalIndex)].category);
    localIndexOut = 0;
    for (int i = 0; i < globalIndex; ++i)
        if (toJuce (all[static_cast<std::size_t> (i)].category) == categoryOut)
            ++localIndexOut;
}

int PresetManager::getNumPresets() noexcept
{
    return static_cast<int> (getFactoryPresets().size());
}

} // namespace prophetrev2::presets
