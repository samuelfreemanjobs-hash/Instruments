#pragma once

#include <JuceHeader.h>

namespace wave9090::ui::tokens
{

inline juce::Colour cyanGlow() noexcept { return juce::Colour (0xff00f0d0); }
inline juce::Colour amberGlow() noexcept { return juce::Colour (0xffff9900); }
inline juce::Colour signalBlue() noexcept { return juce::Colour (0xff3877ff); }
inline juce::Colour panicRed() noexcept { return juce::Colour (0xffe62e2d); }
inline juce::Colour feltCrimson() noexcept { return juce::Colour (0xff8a1818); }

inline juce::Colour bgChassis() noexcept { return juce::Colour (0xff0b0e14); }
inline juce::Colour surfacePanel() noexcept { return juce::Colour (0xff101319); }
inline juce::Colour surfaceInset() noexcept { return juce::Colour (0xff171b24); }
inline juce::Colour surfaceGlass() noexcept { return juce::Colour (0xff070a0e); }
inline juce::Colour woodDark() noexcept { return juce::Colour (0xff23120b); }
inline juce::Colour woodHi() noexcept { return juce::Colour (0xff542b18); }
inline juce::Colour metalScrew() noexcept { return juce::Colour (0xff4a5263); }

inline juce::Colour textPrimary() noexcept { return juce::Colour (0xfff0f4fc); }
inline juce::Colour textSecondary() noexcept { return juce::Colour (0xff8e9bb0); }
inline juce::Colour textDim() noexcept { return juce::Colour (0xff475063); }

enum class KnobDomain { cyan, amber, blue };

inline juce::Colour domainColour (KnobDomain d) noexcept
{
    switch (d)
    {
        case KnobDomain::amber: return amberGlow();
        case KnobDomain::blue: return signalBlue();
        case KnobDomain::cyan:
        default: return cyanGlow();
    }
}

} // namespace wave9090::ui::tokens