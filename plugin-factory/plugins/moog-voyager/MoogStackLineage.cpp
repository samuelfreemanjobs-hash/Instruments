#include "MoogStackLineage.h"
#include "SynthParameterIds.h"
#include "SynthProcessor.h"

namespace moogstack
{
namespace
{
void setChoice(juce::AudioProcessorValueTreeState& apvts, std::string_view id, int index, int numChoices)
{
    if (auto* param = apvts.getParameter(std::string(id)))
    {
        const float norm = numChoices > 1 ? static_cast<float>(index) / static_cast<float>(numChoices - 1) : 0.0f;
        param->setValueNotifyingHost(norm);
    }
}

void setFloat(juce::AudioProcessorValueTreeState& apvts, std::string_view id, float value)
{
    if (auto* param = apvts.getParameter(std::string(id)))
        param->setValueNotifyingHost(param->convertTo0to1(value));
}
} // namespace

std::string_view lineageName(Lineage id) noexcept
{
    switch (id)
    {
        case Lineage::voyager:
            return "Moog Voyager";
        case Lineage::subPhatty:
            return "Moog Sub Phatty";
        case Lineage::subsequent37:
            return "Moog Subsequent 37";
        case Lineage::modelD:
            return "Minimoog Model D";
    }
    return {};
}

std::string_view lineageShortLabel(Lineage id) noexcept
{
    switch (id)
    {
        case Lineage::voyager:
            return "VOYAGER";
        case Lineage::subPhatty:
            return "SUB PHATTY";
        case Lineage::subsequent37:
            return "SUBSEQUENT 37";
        case Lineage::modelD:
            return "MODEL D";
    }
    return {};
}

Lineage lineageFromChoiceIndex(int index) noexcept
{
    index = juce::jlimit(0, lineageCount() - 1, index);
    return static_cast<Lineage>(index);
}

LineageCoeffs coeffsFor(Lineage id) noexcept
{
    LineageCoeffs c {};
    switch (id)
    {
        case Lineage::voyager:
            c.driftAmount = 1.0f;
            c.cutoffScale = 1.0f;
            c.defaultSubOsc = 0.0f;
            break;
        case Lineage::subPhatty:
            c.driftAmount = 0.35f;
            c.cutoffScale = 0.94f;
            c.mixerDriveScale = 1.25f;
            c.filterDriveScale = 1.35f;
            c.subOscAlwaysOn = true;
            c.defaultSubOsc = 0.72f;
            c.resonanceBias = 0.05f;
            break;
        case Lineage::subsequent37:
            c.driftAmount = 0.55f;
            c.cutoffScale = 1.06f;
            c.mixerDriveScale = 1.12f;
            c.filterDriveScale = 1.15f;
            c.subOscAlwaysOn = true;
            c.defaultSubOsc = 0.55f;
            c.resonanceBias = 0.02f;
            break;
        case Lineage::modelD:
            c.driftAmount = 0.12f;
            c.cutoffScale = 0.90f;
            c.mixerDriveScale = 1.18f;
            c.filterDriveScale = 1.05f;
            c.resonanceBias = -0.04f;
            break;
    }
    return c;
}

void applyLineageTemplate(MoogVoyagerAudioProcessor& processor, Lineage id) noexcept
{
    auto& apvts = processor.getApvts();
    const auto c = coeffsFor(id);

    setFloat(apvts, SynthParamIDs::subOscLevel, c.defaultSubOsc);
    setFloat(apvts, SynthParamIDs::mixerDrive, 2.2f * c.mixerDriveScale);
    setFloat(apvts, SynthParamIDs::filterDrive, 1.35f * c.filterDriveScale);

    switch (id)
    {
        case Lineage::voyager:
            setChoice(apvts, SynthParamIDs::osc3Wave, 2, 4);
            setFloat(apvts, SynthParamIDs::lfoToFilter, 0.12f);
            break;
        case Lineage::subPhatty:
            setChoice(apvts, SynthParamIDs::osc1Wave, 0, 4);
            setChoice(apvts, SynthParamIDs::osc2Wave, 0, 4);
            setFloat(apvts, SynthParamIDs::filterResonance, 0.58f);
            setFloat(apvts, SynthParamIDs::filterCutoff, 900.0f);
            break;
        case Lineage::subsequent37:
            setChoice(apvts, SynthParamIDs::osc1Wave, 0, 4);
            setChoice(apvts, SynthParamIDs::osc2Wave, 1, 4);
            setFloat(apvts, SynthParamIDs::filterCutoff, 2200.0f);
            setFloat(apvts, SynthParamIDs::glideTime, 0.06f);
            break;
        case Lineage::modelD:
            setChoice(apvts, SynthParamIDs::osc1Wave, 0, 4);
            setChoice(apvts, SynthParamIDs::osc2Wave, 0, 4);
            setChoice(apvts, SynthParamIDs::osc3Wave, 0, 4);
            setFloat(apvts, SynthParamIDs::filterResonance, 0.42f);
            setFloat(apvts, SynthParamIDs::filterCutoff, 1600.0f);
            setFloat(apvts, SynthParamIDs::lfoToPitch, 0.0f);
            setFloat(apvts, SynthParamIDs::lfoToFilter, 0.0f);
            break;
    }
    juce::ignoreUnused(c);
}

} // namespace moogstack
