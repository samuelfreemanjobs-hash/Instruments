#pragma once

#include <DesignTokens.h>
#include <juce_graphics/juce_graphics.h>

#include <cmath>
#include <cstring>

#ifndef FACTORY_GUI_USE_SYSTEM_FONTS
 #define FACTORY_GUI_USE_SYSTEM_FONTS 0
#endif

namespace fds {
namespace gui {

inline juce::Colour argb (std::uint32_t c) noexcept
{
    return juce::Colour (c);
}

inline juce::Font makeFont (const typography::Style& style, float scale = 1.0f)
{
    const float height = (float) style.size * scale;
    const char* familyName = (std::strcmp (style.familyKey, "mono") == 0)
                                 ? typography::family::mono.name
                                 : typography::family::sans.name;
    auto opts = juce::FontOptions (familyName, height, juce::Font::plain);
    juce::Font font (opts);
    if (style.weight >= 700)
        font = font.boldened();
    return font;
}

inline juce::String applyCase (const juce::String& text, const char* textCase)
{
    if (std::strcmp (textCase, "upper") == 0)
        return text.toUpperCase();
    return text;
}

/** Format a dB value with U+2212 minus and one decimal; −∞ at numeric floor. */
inline juce::String formatDb (float db, float numericFloorDb = meter::numericFloorDb)
{
    if (db <= numericFloorDb || ! std::isfinite (db))
        return juce::String::fromUTF8 ("\xe2\x88\x92\xe2\x88\x9e"); // −∞
    const float v = db;
    juce::String s;
    if (v < 0.0f)
    {
        s = juce::String::fromUTF8 ("\xe2\x88\x92") + juce::String (std::abs (v), 1);
    }
    else if (v > 0.0f)
    {
        s = "+" + juce::String (v, 1);
    }
    else
    {
        s = "0.0";
    }
    return s + " dB";
}

inline float dbToNorm (float db, float floorDb = meter::defaultFloorDb) noexcept
{
    if (db <= floorDb)
        return 0.0f;
    if (db >= 0.0f)
        return 1.0f;
    return (db - floorDb) / (0.0f - floorDb);
}

} // namespace gui
} // namespace fds
