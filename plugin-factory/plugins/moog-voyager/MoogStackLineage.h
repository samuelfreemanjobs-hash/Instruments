#pragma once

#include <string_view>

class MoogVoyagerAudioProcessor;

namespace moogstack
{
/** Four Moog keyboards in one engine: Voyager · Sub Phatty · Subsequent 37 · Model D. */
enum class Lineage : int
{
    voyager = 0,
    subPhatty = 1,
    subsequent37 = 2,
    modelD = 3,
};

constexpr int lineageCount() noexcept
{
    return 4;
}

std::string_view lineageName(Lineage id) noexcept;
std::string_view lineageShortLabel(Lineage id) noexcept;
Lineage lineageFromChoiceIndex(int index) noexcept;

/** Per-lineage DSP coloring (real-time safe coefficients). */
struct LineageCoeffs
{
    float driftAmount = 1.0f;
    float cutoffScale = 1.0f;
    float resonanceBias = 0.0f;
    float mixerDriveScale = 1.0f;
    float filterDriveScale = 1.0f;
    bool subOscAlwaysOn = false;
    float defaultSubOsc = 0.0f;
};

LineageCoeffs coeffsFor(Lineage id) noexcept;

/** Apply characteristic starting-point tweaks when the user picks a stack (does not change program #). */
void applyLineageTemplate(MoogVoyagerAudioProcessor& processor, Lineage id) noexcept;

} // namespace moogstack
