#include "VoyagerPresetBank.h"
#include "SynthParameterIds.h"
#include "SynthProcessor.h"

namespace voyager
{
namespace
{
struct Snapshot
{
    const char* name;
    float oscLevel[3];
    int oscWave[3];
    int oscOctave[3];
    float oscFine[3];
    float noise;
    float mixerDrive;
    float cutoff;
    float resonance;
    float filterDrive;
    float filterEnvAmount;
    float filterKeyTrack;
    float fA, fD, fS, fR;
    float aA, aD, aS, aR;
    float lfoRate;
    float lfoToPitch;
    float lfoToFilter;
    float glide;
    float masterDb;
};

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

void applySnapshot(MoogVoyagerAudioProcessor& processor, const Snapshot& s)
{
    auto& apvts = processor.getApvts();
    const std::string_view waves[3] = { SynthParamIDs::osc1Wave, SynthParamIDs::osc2Wave, SynthParamIDs::osc3Wave };
    const std::string_view octs[3] = { SynthParamIDs::osc1Octave, SynthParamIDs::osc2Octave, SynthParamIDs::osc3Octave };
    const std::string_view fines[3] = { SynthParamIDs::osc1Fine, SynthParamIDs::osc2Fine, SynthParamIDs::osc3Fine };
    const std::string_view levels[3] = { SynthParamIDs::osc1Level, SynthParamIDs::osc2Level, SynthParamIDs::osc3Level };

    for (int i = 0; i < 3; ++i)
    {
        setFloat(apvts, levels[i], s.oscLevel[i]);
        setChoice(apvts, waves[i], s.oscWave[i], 4);
        setChoice(apvts, octs[i], s.oscOctave[i], 5);
        setFloat(apvts, fines[i], s.oscFine[i]);
    }

    setFloat(apvts, SynthParamIDs::noiseLevel, s.noise);
    setFloat(apvts, SynthParamIDs::mixerDrive, s.mixerDrive);
    setFloat(apvts, SynthParamIDs::filterCutoff, s.cutoff);
    setFloat(apvts, SynthParamIDs::filterResonance, s.resonance);
    setFloat(apvts, SynthParamIDs::filterDrive, s.filterDrive);
    setFloat(apvts, SynthParamIDs::filterEnvAmount, s.filterEnvAmount);
    setFloat(apvts, SynthParamIDs::filterKeyTrack, s.filterKeyTrack);
    setFloat(apvts, SynthParamIDs::filterAttack, s.fA);
    setFloat(apvts, SynthParamIDs::filterDecay, s.fD);
    setFloat(apvts, SynthParamIDs::filterSustain, s.fS);
    setFloat(apvts, SynthParamIDs::filterRelease, s.fR);
    setFloat(apvts, SynthParamIDs::ampAttack, s.aA);
    setFloat(apvts, SynthParamIDs::ampDecay, s.aD);
    setFloat(apvts, SynthParamIDs::ampSustain, s.aS);
    setFloat(apvts, SynthParamIDs::ampRelease, s.aR);
    setFloat(apvts, SynthParamIDs::lfoRate, s.lfoRate);
    setFloat(apvts, SynthParamIDs::lfoToPitch, s.lfoToPitch);
    setFloat(apvts, SynthParamIDs::lfoToFilter, s.lfoToFilter);
    setFloat(apvts, SynthParamIDs::glideTime, s.glide);
    setFloat(apvts, SynthParamIDs::outputGainDb, s.masterDb);
}

#include "VoyagerPresetsGenerated.inc"

} // namespace

int getFactoryPresetCount() noexcept
{
    return kCount;
}

std::string_view getFactoryPresetName(int index) noexcept
{
    if (index < 0 || index >= kCount)
        return {};
    return kFactory[static_cast<size_t>(index)].name;
}

void applyFactoryPreset(MoogVoyagerAudioProcessor& processor, int index) noexcept
{
    index = juce::jlimit(0, kCount - 1, index);
    applySnapshot(processor, kFactory[static_cast<size_t>(index)]);
}

} // namespace voyager
