#include "GoldenScenarios.h"

#include "PluginProcessor.h"
#include "Parameters/ParameterIds.h"

namespace junovax::golden
{
namespace
{
using PID = junovax::ParameterIDs;

void setF (JunovaXAudioProcessor& proc, const char* id, float value)
{
    if (auto* param = proc.getApvts().getParameter (id))
        param->setValueNotifyingHost (param->convertTo0to1 (value));
}

void setB (JunovaXAudioProcessor& proc, const char* id, bool value)
{
    if (auto* param = proc.getApvts().getParameter (id))
        param->setValueNotifyingHost (value ? 1.0f : 0.0f);
}

void setChoice (JunovaXAudioProcessor& proc, const char* id, int index, int numChoices)
{
    if (auto* param = proc.getApvts().getParameter (id))
        param->setValueNotifyingHost (static_cast<float> (index) / static_cast<float> (numChoices - 1));
}

void baselineGolden (JunovaXAudioProcessor& proc)
{
    proc.applyFactoryPreset (0);
    setF (proc, PID::arpRate, 0.0f);
    setF (proc, PID::drift, 0.0f);
    setF (proc, PID::dcoNoise, 0.0f);
    setF (proc, PID::masterGain, -3.0f);
}

void applyAb01 (JunovaXAudioProcessor& proc)
{
    baselineGolden (proc);
    setChoice (proc, PID::chorusMode, 0, 4);
    setF (proc, PID::filterCutoff, 0.95f);
    setF (proc, PID::filterRes, 0.05f);
    setF (proc, PID::ampSustain, 1.0f);
    setF (proc, PID::ampRelease, 0.8f);
    setF (proc, PID::dcoPwm, 50.0f);
    setF (proc, PID::dcoSubLvl, 0.0f);
    setChoice (proc, PID::voiceMode, 0, 5);
}

void applyAb02 (JunovaXAudioProcessor& proc)
{
    baselineGolden (proc);
    setChoice (proc, PID::chorusMode, 1, 4);
    setF (proc, PID::filterCutoff, 0.48f);
    setF (proc, PID::vcfEnv, 40.0f);
    setF (proc, PID::dcoPwm, 72.0f);
    setF (proc, PID::dcoSubLvl, 55.0f);
    setF (proc, PID::ampAttack, 0.08f);
    setF (proc, PID::ampRelease, 1.2f);
    setF (proc, PID::width, 130.0f);
}

void applyAb03Chorus (JunovaXAudioProcessor& proc, int modeIndex)
{
    baselineGolden (proc);
    setChoice (proc, PID::chorusMode, modeIndex, 4);
    setF (proc, PID::filterCutoff, 0.72f);
    setF (proc, PID::dcoPwm, 60.0f);
    setF (proc, PID::ampSustain, 0.85f);
}

void applyAb04 (JunovaXAudioProcessor& proc)
{
    baselineGolden (proc);
    setChoice (proc, PID::chorusMode, 0, 4);
    setF (proc, PID::filterRes, 0.5f);
    setF (proc, PID::filterCutoff, 0.35f);
    setF (proc, PID::vcfEnv, 75.0f);
    setF (proc, PID::vcfKey, 100.0f);
    setF (proc, PID::filtDecay, 0.55f);
    setF (proc, PID::filtSustain, 0.15f);
}

void applyAb05 (JunovaXAudioProcessor& proc)
{
    baselineGolden (proc);
    setF (proc, PID::arpRate, 0.35f);
    setF (proc, PID::arpRange, 1.0f);
    setB (proc, PID::arpLatch, false);
    setChoice (proc, PID::chorusMode, 1, 4);
}

void applyAb06 (JunovaXAudioProcessor& proc)
{
    baselineGolden (proc);
    setB (proc, PID::hpfEnabled, true);
    setF (proc, PID::hpfCutoff, 0.35f);
    setF (proc, PID::dcoNoise, 28.0f);
    setChoice (proc, PID::chorusMode, 0, 4);
    setF (proc, PID::filterCutoff, 0.7f);
}

void applyAb07 (JunovaXAudioProcessor& proc)
{
    baselineGolden (proc);
    setChoice (proc, PID::voiceMode, 2, 5);
    setChoice (proc, PID::chorusMode, 2, 4);
    setF (proc, PID::detune, 14.0f);
    setF (proc, PID::filterCutoff, 0.62f);
    setF (proc, PID::filterRes, 0.22f);
}

void applyAbJuno6 (JunovaXAudioProcessor& proc)
{
    baselineGolden (proc);
    setChoice (proc, PID::voiceMode, 4, 5);
    setChoice (proc, PID::chorusMode, 1, 4);
    setF (proc, PID::filterCutoff, 0.58f);
}
} // namespace

bool applyScenario (JunovaXAudioProcessor& processor, const char* scenarioId) noexcept
{
    if (scenarioId == nullptr)
        return false;

    const juce::String id (scenarioId);
    if (id == "ab01-dry-saw")
        applyAb01 (processor);
    else if (id == "ab02-fat-pad")
        applyAb02 (processor);
    else if (id == "ab03-chorus-i")
        applyAb03Chorus (processor, 1);
    else if (id == "ab03-chorus-ii")
        applyAb03Chorus (processor, 2);
    else if (id == "ab03-chorus-i-ii")
        applyAb03Chorus (processor, 3);
    else if (id == "ab04-filter-sweep")
        applyAb04 (processor);
    else if (id == "ab05-arp-sync")
        applyAb05 (processor);
    else if (id == "ab06-noise-hpf")
        applyAb06 (processor);
    else if (id == "ab07-unison-lead")
        applyAb07 (processor);
    else if (id == "ab-juno6-poly")
        applyAbJuno6 (processor);
    else
        return false;

    return true;
}

const char* listScenarioIds() noexcept
{
    return "ab01-dry-saw ab02-fat-pad ab03-chorus-i ab03-chorus-ii ab03-chorus-i-ii "
           "ab04-filter-sweep ab05-arp-sync ab06-noise-hpf ab07-unison-lead ab-juno6-poly";
}
} // namespace junovax::golden
