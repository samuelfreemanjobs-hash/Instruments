#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace prophetrev2::presets
{

/** User-local favorite factory indices and user preset names (not in host project state). */
class FavoritesStore
{
public:
    static bool isFactoryFavorite (int factoryGlobalIndex);
    static void setFactoryFavorite (int factoryGlobalIndex, bool favorite);

    static bool isUserFavorite (const juce::String& presetName);
    static void setUserFavorite (const juce::String& presetName, bool favorite);

private:
    static juce::File storeFile();
    static juce::ValueTree loadTree();
    static bool saveTree (const juce::ValueTree& root);
};

} // namespace prophetrev2::presets
