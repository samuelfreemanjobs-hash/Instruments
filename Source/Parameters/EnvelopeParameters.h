#pragma once

#include <juce_core/juce_core.h>

namespace jdupgraded::params
{

inline constexpr const char* kEnvelopeLinkId = "envelopeLink";

inline constexpr const char* kGlobalAmpAttackId = "ampAttack";
inline constexpr const char* kGlobalAmpDecayId = "ampDecay";
inline constexpr const char* kGlobalAmpSustainId = "ampSustain";
inline constexpr const char* kGlobalAmpReleaseId = "ampRelease";
inline constexpr const char* kGlobalFilterAttackId = "filterAttack";
inline constexpr const char* kGlobalFilterDecayId = "filterDecay";
inline constexpr const char* kGlobalFilterSustainId = "filterSustain";
inline constexpr const char* kGlobalFilterReleaseId = "filterRelease";

inline juce::String toneEnvelopeParamId (int toneIndex1Based, const char* suffix) noexcept
{
    return "tone" + juce::String (toneIndex1Based) + suffix;
}

} // namespace jdupgraded::params
