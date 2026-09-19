#include "GfunkPresets.h"

namespace gfunk
{
namespace
{
void setChoice(juce::AudioProcessorValueTreeState& apvts, std::string_view id, int index, int numChoices)
{
    if (auto* param = apvts.getParameter(std::string(id)))
    {
        const float norm = numChoices > 1 ? static_cast<float>(index) / static_cast<float>(numChoices - 1)
                                          : 0.0f;
        param->setValueNotifyingHost(norm);
    }
}

void setFloat(juce::AudioProcessorValueTreeState& apvts, std::string_view id, float value)
{
    if (auto* param = apvts.getParameter(std::string(id)))
        param->setValueNotifyingHost(param->convertTo0to1(value));
}
} // namespace

void applyPreset(MoogVoyagerAudioProcessor& processor, PresetId preset)
{
    auto& apvts = processor.getApvts();

    if (preset == PresetId::gFunkLead)
    {
        setFloat(apvts, SynthParamIDs::osc1Level, 0.78f);
        setFloat(apvts, SynthParamIDs::osc2Level, 0.62f);
        setFloat(apvts, SynthParamIDs::osc3Level, 0.48f);
        setChoice(apvts, SynthParamIDs::osc1Wave, 0, 4);
        setChoice(apvts, SynthParamIDs::osc2Wave, 0, 4);
        setChoice(apvts, SynthParamIDs::osc3Wave, 1, 4);
        setChoice(apvts, SynthParamIDs::osc1Octave, 2, 5);
        setChoice(apvts, SynthParamIDs::osc2Octave, 2, 5);
        setChoice(apvts, SynthParamIDs::osc3Octave, 3, 5);
        setFloat(apvts, SynthParamIDs::osc1Fine, 0.0f);
        setFloat(apvts, SynthParamIDs::osc2Fine, 9.0f);
        setFloat(apvts, SynthParamIDs::osc3Fine, -11.0f);
        setFloat(apvts, SynthParamIDs::noiseLevel, 0.02f);
        setFloat(apvts, SynthParamIDs::mixerDrive, 2.35f);
        setFloat(apvts, SynthParamIDs::filterCutoff, 1800.0f);
        setFloat(apvts, SynthParamIDs::filterResonance, 0.58f);
        setFloat(apvts, SynthParamIDs::filterDrive, 1.55f);
        setFloat(apvts, SynthParamIDs::filterEnvAmount, 0.82f);
        setFloat(apvts, SynthParamIDs::filterKeyTrack, 0.62f);
        setFloat(apvts, SynthParamIDs::filterAttack, 0.004f);
        setFloat(apvts, SynthParamIDs::filterDecay, 0.22f);
        setFloat(apvts, SynthParamIDs::filterSustain, 0.35f);
        setFloat(apvts, SynthParamIDs::filterRelease, 0.45f);
        setFloat(apvts, SynthParamIDs::ampAttack, 0.004f);
        setFloat(apvts, SynthParamIDs::ampDecay, 0.18f);
        setFloat(apvts, SynthParamIDs::ampSustain, 0.88f);
        setFloat(apvts, SynthParamIDs::ampRelease, 0.35f);
        setFloat(apvts, SynthParamIDs::lfoRate, 0.35f);
        setFloat(apvts, SynthParamIDs::lfoToPitch, 0.04f);
        setFloat(apvts, SynthParamIDs::lfoToFilter, 0.08f);
        setFloat(apvts, SynthParamIDs::glideTime, 0.14f);
        setFloat(apvts, SynthParamIDs::outputGainDb, -1.5f);
        return;
    }

    // G-Funk Bass
    setFloat(apvts, SynthParamIDs::osc1Level, 0.95f);
    setFloat(apvts, SynthParamIDs::osc2Level, 0.55f);
    setFloat(apvts, SynthParamIDs::osc3Level, 0.35f);
    setChoice(apvts, SynthParamIDs::osc1Wave, 0, 4);
    setChoice(apvts, SynthParamIDs::osc2Wave, 0, 4);
    setChoice(apvts, SynthParamIDs::osc3Wave, 0, 4);
    setChoice(apvts, SynthParamIDs::osc1Octave, 2, 5);
    setChoice(apvts, SynthParamIDs::osc2Octave, 2, 5);
    setChoice(apvts, SynthParamIDs::osc3Octave, 2, 5);
    setFloat(apvts, SynthParamIDs::noiseLevel, 0.05f);
    setFloat(apvts, SynthParamIDs::mixerDrive, 2.1f);
    setFloat(apvts, SynthParamIDs::filterCutoff, 520.0f);
    setFloat(apvts, SynthParamIDs::filterResonance, 0.62f);
    setFloat(apvts, SynthParamIDs::filterEnvAmount, 0.55f);
    setFloat(apvts, SynthParamIDs::glideTime, 0.11f);
    setFloat(apvts, SynthParamIDs::outputGainDb, 2.0f);
}

} // namespace gfunk
