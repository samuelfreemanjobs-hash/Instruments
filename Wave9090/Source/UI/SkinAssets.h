#pragma once

#include <JuceHeader.h>

namespace wave9090::ui
{

/** Loads skin images from JUCE BinaryData — replace PNGs in Assets/skin/ and rebuild. */
class SkinAssets
{
public:
    static juce::Image loadPanelBackground();
    static juce::Image loadKnobDial();
    static juce::Image loadHeaderStrip();

    static juce::Image decode (const void* data, int size);
};

} // namespace wave9090::ui