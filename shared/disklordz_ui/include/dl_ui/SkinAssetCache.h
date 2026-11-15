#pragma once

#include <juce_graphics/juce_graphics.h>

#include <unordered_map>

namespace dl_ui
{

/** Loads PNG filmstrips and backgrounds from JUCE BinaryData symbols. */
class SkinAssetCache final
{
public:
    juce::Image getImage (const juce::String& resourceName);
    juce::Image getFilmstripFrame (const juce::String& resourceName, int frameIndex, int numFrames);

    void registerBinary (const juce::String& name, const void* data, int size);

private:
    std::unordered_map<juce::String, juce::Image> cache_;
};

} // namespace dl_ui