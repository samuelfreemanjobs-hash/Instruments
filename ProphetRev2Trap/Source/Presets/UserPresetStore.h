#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace prophetrev2::presets
{

inline constexpr const char* kUserPresetCategory = "User";

class UserPresetStore
{
public:
    static juce::File presetsDirectory();

    static juce::StringArray listPresetNames();
    static bool savePreset (const juce::String& name, const juce::ValueTree& apvtsState);
    static bool loadPreset (const juce::String& name, juce::ValueTree& apvtsStateOut);
    static bool deletePreset (const juce::String& name);

private:
    static juce::File fileForName (const juce::String& name);
    static juce::String sanitiseName (const juce::String& name);
};

} // namespace prophetrev2::presets
