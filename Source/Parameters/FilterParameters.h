#pragma once

#include <juce_core/juce_core.h>

namespace jdupgraded::params
{

inline constexpr const char* kFilterLinkId = "filterLink";

inline juce::String toneFilterParamId (int toneIndex1Based, const char* suffix) noexcept
{
    return "tone" + juce::String (toneIndex1Based) + "Filter" + suffix;
}

} // namespace jdupgraded::params
