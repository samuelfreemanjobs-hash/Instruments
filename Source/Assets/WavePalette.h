#pragma once

#include "RomFormat.h"

#include <juce_graphics/juce_graphics.h>

#include <cstddef>

namespace jdupgraded::assets
{

inline const char* categoryDisplayName (RomWaveCategory category) noexcept
{
    switch (category)
    {
        case RomWaveCategory::analog: return "Analog";
        case RomWaveCategory::digital: return "Digital";
        case RomWaveCategory::bell: return "Bell";
        case RomWaveCategory::pluck: return "Pluck";
        case RomWaveCategory::bass: return "Bass";
        case RomWaveCategory::noise: return "Noise";
        case RomWaveCategory::vocal: return "Vocal";
        case RomWaveCategory::organ: return "Organ";
        case RomWaveCategory::fx: return "FX";
        default: return "Wave";
    }
}

inline juce::Colour categoryAccentColour (RomWaveCategory category) noexcept
{
    switch (category)
    {
        case RomWaveCategory::analog: return juce::Colour (0xff6ea8fe);
        case RomWaveCategory::digital: return juce::Colour (0xffa78bfa);
        case RomWaveCategory::bell: return juce::Colour (0xfffbbf24);
        case RomWaveCategory::pluck: return juce::Colour (0xff34d399);
        case RomWaveCategory::bass: return juce::Colour (0xfff87171);
        case RomWaveCategory::noise: return juce::Colour (0xff9ca3af);
        case RomWaveCategory::vocal: return juce::Colour (0xfff472b6);
        case RomWaveCategory::organ: return juce::Colour (0xfffb923c);
        case RomWaveCategory::fx: return juce::Colour (0xff22d3ee);
        default: return juce::Colours::white.withAlpha (0.5f);
    }
}

/** Fallback when ROM metadata is unavailable. */
inline RomWaveCategory categoryForWaveIndexFallback (std::size_t waveIndex) noexcept
{
    if (waveIndex < kSingleWaveCount)
        return static_cast<RomWaveCategory> (waveIndex % 9);

    const std::size_t msIndex = (waveIndex - kSingleWaveCount) / kZonesPerSet;
    const std::uint32_t set = static_cast<std::uint32_t> (msIndex) + 1;
    if (set <= 4)
        return RomWaveCategory::analog;
    if (set <= 7)
        return RomWaveCategory::digital;
    if (set <= 10)
        return RomWaveCategory::bell;
    if (set <= 13)
        return RomWaveCategory::pluck;
    if (set <= 16)
        return RomWaveCategory::bass;
    if (set <= 18)
        return RomWaveCategory::noise;
    if (set <= 20)
        return RomWaveCategory::vocal;
    if (set <= 22)
        return RomWaveCategory::organ;
    return RomWaveCategory::fx;
}

} // namespace jdupgraded::assets
