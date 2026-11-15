#pragma once

#include <JuceHeader.h>

namespace junovax::ui::celestial
{
/** Sam design-system tokens (mockup v1). */
struct Theme
{
    static inline juce::Colour background() { return juce::Colour (0xff090d16); }
    static inline juce::Colour obsidian() { return background(); }
    static inline juce::Colour electricCyan() { return juce::Colour (0xff00d2ff); }
    static inline juce::Colour stellarCrimson() { return juce::Colour (0xffff334b); }
    static inline juce::Colour specularChrome() { return juce::Colour (0xfff8fafc); }
    static inline juce::Colour ledInactive() { return juce::Colour (0xff1e293b); }

    static inline juce::Colour panelBlue() { return juce::Colour (0xff0c1524); }
    static inline juce::Colour panelRed() { return juce::Colour (0xff1a0a10); }
    static inline juce::Colour panelGrey() { return juce::Colour (0xff0e1219); }

    static inline juce::Colour glowBlue() { return electricCyan(); }
    static inline juce::Colour glowRed() { return stellarCrimson(); }
    static inline juce::Colour glowCyan() { return electricCyan(); }
    static inline juce::Colour textPrimary() { return specularChrome(); }
    static inline juce::Colour textMuted() { return ledInactive().brighter (0.35f); }

    static juce::Font headerFont (float heightPx)
    {
        return juce::Font (juce::Font::getDefaultSansSerifFontName(), heightPx, juce::Font::bold);
    }

    static juce::Font readoutFont (float heightPx)
    {
        return juce::Font (juce::Font::getDefaultMonospacedFontName(), heightPx, juce::Font::plain);
    }

    static juce::Font labelFont()
    {
        return juce::Font (juce::Font::getDefaultSansSerifFontName(), 9.0f, juce::Font::plain);
    }
};
} // namespace junovax::ui::celestial