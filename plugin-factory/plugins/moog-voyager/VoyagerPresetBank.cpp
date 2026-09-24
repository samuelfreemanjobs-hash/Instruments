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
    float filterEnvAmt;
    float filterKb;
    float fA, fD, fS, fR;
    float aA, aD, aS, aR;
    float lfoRate;
    float lfoPitch;
    float lfoFilter;
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
    setFloat(apvts, SynthParamIDs::filterEnvAmount, s.filterEnvAmt);
    setFloat(apvts, SynthParamIDs::filterKeyTrack, s.filterKb);
    setFloat(apvts, SynthParamIDs::filterAttack, s.fA);
    setFloat(apvts, SynthParamIDs::filterDecay, s.fD);
    setFloat(apvts, SynthParamIDs::filterSustain, s.fS);
    setFloat(apvts, SynthParamIDs::filterRelease, s.fR);
    setFloat(apvts, SynthParamIDs::ampAttack, s.aA);
    setFloat(apvts, SynthParamIDs::ampDecay, s.aD);
    setFloat(apvts, SynthParamIDs::ampSustain, s.aS);
    setFloat(apvts, SynthParamIDs::ampRelease, s.aR);
    setFloat(apvts, SynthParamIDs::lfoRate, s.lfoRate);
    setFloat(apvts, SynthParamIDs::lfoToPitch, s.lfoPitch);
    setFloat(apvts, SynthParamIDs::lfoToFilter, s.lfoFilter);
    setFloat(apvts, SynthParamIDs::glideTime, s.glide);
    setFloat(apvts, SynthParamIDs::outputGainDb, s.masterDb);
}

// clang-format off
const Snapshot kFactory[] = {
    // name, levels[3], waves[3], oct[3], fine[3], noise, mix, cut, res, fdrive, fenv, fkb,
    // fA,fD,fS,fR, aA,aD,aS,aR, lfoR, lfoP, lfoF, glide, master
    { "01 Voyager Init",
      {0.75f,0.50f,0.40f}, {0,0,2}, {2,2,2}, {0,7,-7},
      0.03f, 2.0f, 2400.f, 0.45f, 1.4f, 0.65f, 0.45f,
      0.01f,0.30f,0.30f,0.50f, 0.005f,0.20f,0.85f,0.45f, 0.5f,0,0.12f, 0.05f, -2.f },

    { "02 Classic Mono Lead",
      {0.85f,0.45f,0.30f}, {0,0,1}, {2,2,3}, {0,12,-8},
      0.02f, 2.3f, 3200.f, 0.52f, 1.5f, 0.75f, 0.55f,
      0.003f,0.18f,0.40f,0.40f, 0.003f,0.15f,0.90f,0.35f, 0.4f,0.02f,0.10f, 0.08f, -1.5f },

    { "03 G-Funk Lead",
      {0.78f,0.62f,0.48f}, {0,0,1}, {2,2,3}, {0,9,-11},
      0.02f, 2.35f, 1800.f, 0.58f, 1.55f, 0.82f, 0.62f,
      0.004f,0.22f,0.35f,0.45f, 0.004f,0.18f,0.88f,0.35f, 0.35f,0.04f,0.08f, 0.14f, -1.5f },

    { "04 Cali Glide Lead",
      {0.80f,0.55f,0.35f}, {0,0,0}, {2,2,2}, {0,15,-15},
      0.01f, 2.2f, 2600.f, 0.50f, 1.45f, 0.70f, 0.50f,
      0.008f,0.25f,0.45f,0.50f, 0.006f,0.20f,0.92f,0.50f, 0.25f,0,0.05f, 0.45f, -2.f },

    { "05 Bright Reso Lead",
      {0.70f,0.65f,0.55f}, {0,1,1}, {2,3,3}, {0,5,5},
      0.01f, 2.5f, 4500.f, 0.72f, 1.6f, 0.60f, 0.70f,
      0.002f,0.15f,0.55f,0.35f, 0.002f,0.12f,0.90f,0.30f, 0.6f,0.03f,0.15f, 0.06f, -2.f },

    { "06 Square Sizzle",
      {0.55f,0.80f,0.40f}, {1,1,0}, {2,2,4}, {0,0,20},
      0.04f, 2.6f, 3800.f, 0.48f, 1.7f, 0.55f, 0.60f,
      0.005f,0.20f,0.50f,0.40f, 0.004f,0.16f,0.88f,0.35f, 0.8f,0.05f,0.20f, 0.10f, -1.f },

    { "07 G-Funk Bass",
      {0.95f,0.55f,0.35f}, {0,0,0}, {2,2,2}, {0,0,0},
      0.05f, 2.1f, 520.f, 0.62f, 1.85f, 0.55f, 0.40f,
      0.005f,0.35f,0.25f,0.50f, 0.003f,0.18f,0.92f,0.35f, 0.3f,0,0.05f, 0.11f, 2.f },

    { "08 Sub Foundation",
      {1.0f,0.35f,0.20f}, {0,0,0}, {1,2,2}, {0,0,0},
      0.02f, 2.0f, 380.f, 0.55f, 1.9f, 0.45f, 0.35f,
      0.008f,0.40f,0.20f,0.55f, 0.004f,0.25f,0.95f,0.40f, 0.2f,0,0, 0.05f, 3.f },

    { "09 Funk Pick Bass",
      {0.88f,0.60f,0.25f}, {0,1,0}, {2,2,2}, {0,8,-5},
      0.06f, 2.4f, 680.f, 0.58f, 2.0f, 0.65f, 0.45f,
      0.002f,0.22f,0.15f,0.35f, 0.001f,0.12f,0.85f,0.25f, 0.4f,0,0.08f, 0.07f, 1.5f },

    { "10 Rubber Bass",
      {0.90f,0.50f,0.30f}, {0,2,0}, {2,2,1}, {0,-10,5},
      0.03f, 1.9f, 900.f, 0.50f, 1.5f, 0.50f, 0.50f,
      0.015f,0.45f,0.35f,0.60f, 0.010f,0.30f,0.90f,0.55f, 0.15f,0,0.03f, 0.12f, 0.f },

    { "11 Deep Sub Drive",
      {0.92f,0.45f,0.15f}, {0,0,2}, {1,2,2}, {0,0,0},
      0.08f, 2.8f, 420.f, 0.65f, 2.2f, 0.40f, 0.30f,
      0.010f,0.50f,0.18f,0.60f, 0.005f,0.30f,0.98f,0.45f, 0.1f,0,0, 0.04f, 4.f },

    { "12 Silk Pad",
      {0.55f,0.55f,0.50f}, {2,2,2}, {2,2,3}, {0,7,-7},
      0.01f, 1.6f, 1400.f, 0.35f, 1.2f, 0.35f, 0.55f,
      0.80f,1.20f,0.70f,1.50f, 0.60f,0.80f,0.95f,1.80f, 0.12f,0.02f,0.25f, 0.20f, -3.f },

    { "13 Voyager Dream",
      {0.60f,0.48f,0.42f}, {0,2,2}, {2,2,4}, {0,11,-11},
      0.02f, 1.7f, 1100.f, 0.42f, 1.25f, 0.40f, 0.60f,
      1.00f,1.50f,0.75f,2.00f, 0.80f,1.00f,0.98f,2.20f, 0.08f,0.03f,0.30f, 0.25f, -3.5f },

    { "14 Dark Atlas Pad",
      {0.50f,0.45f,0.55f}, {0,0,2}, {1,2,3}, {0,-5,5},
      0.04f, 2.0f, 850.f, 0.38f, 1.4f, 0.30f, 0.45f,
      0.50f,0.90f,0.65f,1.20f, 0.40f,0.70f,0.92f,1.50f, 0.20f,0,0.35f, 0.18f, -2.f },

    { "15 Soft Triangle Keys",
      {0.65f,0.40f,0.35f}, {2,2,2}, {2,2,2}, {0,3,-3},
      0.01f, 1.5f, 3200.f, 0.28f, 1.15f, 0.25f, 0.65f,
      0.02f,0.35f,0.45f,0.50f, 0.008f,0.25f,0.75f,0.55f, 0.3f,0,0.05f, 0.02f, -2.f },

    { "16 Brass Stab",
      {0.70f,0.75f,0.45f}, {0,1,1}, {2,2,2}, {0,5,0},
      0.02f, 2.7f, 2200.f, 0.55f, 2.1f, 0.90f, 0.55f,
      0.001f,0.25f,0.05f,0.30f, 0.001f,0.20f,0.70f,0.25f, 0.5f,0,0.20f, 0.03f, -1.f },

    { "17 Pulse Stepper",
      {0.50f,0.70f,0.60f}, {3,3,1}, {2,2,3}, {0,0,0},
      0.01f, 2.2f, 1600.f, 0.40f, 1.5f, 0.50f, 0.50f,
      0.005f,0.15f,0.30f,0.25f, 0.003f,0.10f,0.80f,0.20f, 1.2f,0,0.40f, 0.05f, -2.f },

    { "18 Filter Sweep FX",
      {0.40f,0.40f,0.40f}, {0,0,0}, {2,2,2}, {0,0,0},
      0.12f, 2.0f, 600.f, 0.75f, 1.8f, 0.95f, 0.20f,
      0.05f,2.50f,0.10f,1.80f, 0.02f,1.50f,0.90f,1.50f, 0.25f,0,0.85f, 0.02f, -3.f },

    { "19 LFO Wobble",
      {0.65f,0.55f,0.45f}, {0,0,2}, {2,2,2}, {0,10,-10},
      0.02f, 2.1f, 1200.f, 0.48f, 1.45f, 0.35f, 0.40f,
      0.01f,0.30f,0.40f,0.50f, 0.008f,0.22f,0.88f,0.45f, 2.5f,0.08f,0.75f, 0.10f, -2.f },

    { "20 Noise Rumble",
      {0.35f,0.30f,0.25f}, {0,0,2}, {1,2,2}, {0,0,0},
      0.45f, 2.5f, 450.f, 0.60f, 2.0f, 0.50f, 0.25f,
      0.02f,0.60f,0.30f,0.80f, 0.010f,0.40f,0.95f,0.70f, 0.15f,0,0.10f, 0.04f, 0.f },

    { "21 Filter Whistle",
      {0.30f,0.30f,0.25f}, {2,2,2}, {3,3,4}, {0,0,0},
      0.05f, 1.8f, 8000.f, 0.92f, 1.3f, 0.20f, 0.80f,
      0.02f,0.40f,0.60f,0.50f, 0.010f,0.30f,0.85f,0.40f, 0.4f,0.10f,0.05f, 0.06f, -4.f },

    { "22 West Coast Pluck",
      {0.75f,0.50f,0.35f}, {0,0,1}, {2,2,3}, {0,14,-14},
      0.02f, 2.4f, 2800.f, 0.55f, 1.65f, 0.85f, 0.58f,
      0.001f,0.18f,0.05f,0.28f, 0.001f,0.15f,0.05f,0.22f, 0.35f,0.02f,0.12f, 0.09f, -1.f },

    { "23 Night Drive",
      {0.68f,0.52f,0.44f}, {0,1,2}, {2,2,2}, {0,-7,7},
      0.03f, 2.3f, 950.f, 0.52f, 1.75f, 0.60f, 0.48f,
      0.012f,0.35f,0.28f,0.45f, 0.008f,0.28f,0.90f,0.40f, 0.45f,0.03f,0.18f, 0.16f, -1.f },

    { "24 Full Stack Unison",
      {0.72f,0.72f,0.68f}, {0,0,1}, {2,2,3}, {0,18,-18},
      0.02f, 2.65f, 3400.f, 0.56f, 1.75f, 0.68f, 0.65f,
      0.004f,0.20f,0.42f,0.38f, 0.003f,0.16f,0.92f,0.32f, 0.55f,0.05f,0.14f, 0.11f, -2.5f },
};
// clang-format on

constexpr int kCount = static_cast<int>(sizeof(kFactory) / sizeof(kFactory[0]));
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
