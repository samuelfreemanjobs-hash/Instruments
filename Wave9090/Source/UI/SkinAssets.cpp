#include "UI/SkinAssets.h"

#include <BinaryData.h>

namespace wave9090::ui
{

juce::Image SkinAssets::decode (const void* data, int size)
{
    if (data == nullptr || size <= 0)
        return {};

    auto stream = juce::MemoryInputStream (data, static_cast<std::size_t> (size), false);
    return juce::ImageFileFormat::loadFrom (stream);
}

juce::Image SkinAssets::loadPanelBackground()
{
    return decode (BinaryData::panel_bg_png, BinaryData::panel_bg_pngSize);
}

juce::Image SkinAssets::loadKnobDial()
{
    return decode (BinaryData::knob_dial_png, BinaryData::knob_dial_pngSize);
}

juce::Image SkinAssets::loadHeaderStrip()
{
    return decode (BinaryData::header_strip_png, BinaryData::header_strip_pngSize);
}

} // namespace wave9090::ui