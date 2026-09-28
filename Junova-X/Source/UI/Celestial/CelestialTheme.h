#pragma once

#include <JuceHeader.h>

namespace junovax::ui::celestial
{
struct Theme
{
    static inline juce::Colour background() { return juce::Colour (0xff07070c); }
    static inline juce::Colour panelBlue() { return juce::Colour (0xff141a2e); }
    static inline juce::Colour panelRed() { return juce::Colour (0xff2a1218); }
    static inline juce::Colour panelGrey() { return juce::Colour (0xff181820); }
    static inline juce::Colour glowBlue() { return juce::Colour (0xff4dabf7); }
    static inline juce::Colour glowRed() { return juce::Colour (0xffff6b6b); }
    static inline juce::Colour glowCyan() { return juce::Colour (0xff22d3ee); }
    static inline juce::Colour textPrimary() { return juce::Colour (0xffe8e8f0); }
    static inline juce::Colour textMuted() { return juce::Colour (0xff8888a0); }
};
} // namespace junovax::ui::celestial
