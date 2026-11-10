#pragma once
#include <JuceHeader.h>
#include <optional>
#include <vector>

struct PresetRecord
{
    juce::String name;
    juce::String category;
    int bank = 0;
    juce::var params;
};

class PresetLibrary
{
public:
    static PresetLibrary& instance();

    bool loadFromBinaryData (const void* data, size_t size);
    int count() const { return int (presets.size()); }
    const PresetRecord& get (int index) const;
    void applyToState (juce::AudioProcessorValueTreeState& state, int index) const;

    juce::StringArray categories() const;
    juce::Array<int> indicesForCategory (const juce::String& category) const;
    juce::Array<int> search (const juce::String& text, const juce::String& categoryFilter) const;

private:
    PresetLibrary() = default;
    std::vector<PresetRecord> presets;
    static PresetRecord empty;
};
