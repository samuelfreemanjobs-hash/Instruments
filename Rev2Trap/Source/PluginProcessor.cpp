#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Presets/Jz400Presets.h"

namespace
{
using PID = rev2trap::ParameterIDs;

juce::AudioProcessorValueTreeState::ParameterLayout createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    auto add = [&] (const char* id, const juce::String& name, juce::NormalisableRange<float> range, float def)
    {
        layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, 1 }, name, range, def));
    };
    add (PID::outputGain, "Output", { -24.0f, 6.0f, 0.01f }, -3.0f);
    add (PID::oscMix, "Osc Mix", { 0.0f, 1.0f, 0.001f }, 0.55f);
    add (PID::osc2Detune, "Osc2 Detune", { -50.0f, 50.0f, 0.01f }, 7.0f);
    add (PID::subLevel, "Sub", { 0.0f, 1.0f, 0.001f }, 0.35f);
    add (PID::cutoff, "Cutoff", { 0.0f, 1.0f, 0.001f }, 0.55f);
    add (PID::resonance, "Resonance", { 0.0f, 1.0f, 0.001f }, 0.28f);
    add (PID::filterEnv, "Filter Env", { 0.0f, 1.0f, 0.001f }, 0.5f);
    add (PID::drive, "Drive", { 0.5f, 2.0f, 0.001f }, 1.0f);
    add (PID::ampAttack, "Amp A", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.008f);
    add (PID::ampDecay, "Amp D", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.35f);
    add (PID::ampSustain, "Amp S", { 0.0f, 1.0f, 0.001f }, 0.55f);
    add (PID::ampRelease, "Amp R", { 0.001f, 5.0f, 0.001f, 0.35f }, 0.45f);
    add (PID::filtAttack, "Filt A", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.005f);
    add (PID::filtDecay, "Filt D", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.4f);
    add (PID::filtSustain, "Filt S", { 0.0f, 1.0f, 0.001f }, 0.25f);
    add (PID::filtRelease, "Filt R", { 0.001f, 5.0f, 0.001f, 0.35f }, 0.5f);
    add (PID::wavetableBlend, "WT Blend", { 0.0f, 1.0f, 0.001f }, 0.35f);
    add (PID::hardSync, "Hard Sync", { 0.0f, 1.0f, 0.001f }, 0.0f);
    add (PID::filterFm, "Filter FM", { 0.0f, 1.0f, 0.001f }, 0.15f);
    add (PID::drift, "Drift", { 0.0f, 1.0f, 0.001f }, 0.1f);
    add (PID::trapMacro, "Trap", { 0.0f, 1.0f, 0.001f }, 0.4f);
    return layout;
}
} // namespace

Rev2TrapAudioProcessor::Rev2TrapAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
#endif
      apvts_ (*this, nullptr, "Rev2Trap", createLayout())
{
    setCurrentProgram (0);
}

Rev2TrapAudioProcessor::~Rev2TrapAudioProcessor() = default;

const juce::String Rev2TrapAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

void Rev2TrapAudioProcessor::prepareToPlay (double sampleRate, int)
{
    for (auto& v : voices_)
        v.prepare (sampleRate);
}

void Rev2TrapAudioProcessor::releaseResources() {}

bool Rev2TrapAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void Rev2TrapAudioProcessor::refreshVoiceParams()
{
    currentParams_ = rev2trap::presets::getPresetForProgram (currentProgram_.load());
    auto get = [this] (const char* id, float fallback) -> float
    {
        if (auto* p = apvts_.getRawParameterValue (id))
            return p->load();
        return fallback;
    };
    currentParams_.oscMix = get (PID::oscMix, currentParams_.oscMix);
    currentParams_.osc2DetuneCents = get (PID::osc2Detune, currentParams_.osc2DetuneCents);
    currentParams_.subLevel = get (PID::subLevel, currentParams_.subLevel);
    currentParams_.cutoff = get (PID::cutoff, currentParams_.cutoff);
    currentParams_.resonance = get (PID::resonance, currentParams_.resonance);
    currentParams_.filterEnv = get (PID::filterEnv, currentParams_.filterEnv);
    currentParams_.drive = get (PID::drive, currentParams_.drive);
    currentParams_.ampAttack = get (PID::ampAttack, currentParams_.ampAttack);
    currentParams_.ampDecay = get (PID::ampDecay, currentParams_.ampDecay);
    currentParams_.ampSustain = get (PID::ampSustain, currentParams_.ampSustain);
    currentParams_.ampRelease = get (PID::ampRelease, currentParams_.ampRelease);
    currentParams_.filtAttack = get (PID::filtAttack, currentParams_.filtAttack);
    currentParams_.filtDecay = get (PID::filtDecay, currentParams_.filtDecay);
    currentParams_.filtSustain = get (PID::filtSustain, currentParams_.filtSustain);
    currentParams_.filtRelease = get (PID::filtRelease, currentParams_.filtRelease);
    currentParams_.wavetableBlend = get (PID::wavetableBlend, currentParams_.wavetableBlend);
    currentParams_.hardSyncAmount = get (PID::hardSync, currentParams_.hardSyncAmount);
    currentParams_.filterFm = get (PID::filterFm, currentParams_.filterFm);
    currentParams_.driftAmount = get (PID::drift, currentParams_.driftAmount);
    currentParams_.trapMacro = get (PID::trapMacro, currentParams_.trapMacro);
}

void Rev2TrapAudioProcessor::applyPreset (int index)
{
    const int clamped = juce::jlimit (0, rev2trap::presets::kTotalFactoryPrograms - 1, index);
    currentProgram_.store (clamped);
    const auto p = rev2trap::presets::getPresetForProgram (clamped);
    auto set = [this] (const char* id, float v)
    {
        if (auto* param = apvts_.getParameter (id))
            param->setValueNotifyingHost (param->convertTo0to1 (v));
    };
    set (PID::oscMix, p.oscMix);
    set (PID::osc2Detune, p.osc2DetuneCents);
    set (PID::subLevel, p.subLevel);
    set (PID::cutoff, p.cutoff);
    set (PID::resonance, p.resonance);
    set (PID::filterEnv, p.filterEnv);
    set (PID::drive, p.drive);
    set (PID::ampAttack, p.ampAttack);
    set (PID::ampDecay, p.ampDecay);
    set (PID::ampSustain, p.ampSustain);
    set (PID::ampRelease, p.ampRelease);
    set (PID::filtAttack, p.filtAttack);
    set (PID::filtDecay, p.filtDecay);
    set (PID::filtSustain, p.filtSustain);
    set (PID::filtRelease, p.filtRelease);
    set (PID::wavetableBlend, p.wavetableBlend);
    set (PID::hardSync, p.hardSyncAmount);
    set (PID::filterFm, p.filterFm);
    set (PID::drift, p.driftAmount);
    set (PID::trapMacro, p.trapMacro);
    refreshVoiceParams();
}

void Rev2TrapAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    const float gain = juce::Decibels::decibelsToGain (apvts_.getRawParameterValue (PID::outputGain)->load());

    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            for (auto& v : voices_)
            {
                if (! v.isActive())
                {
                    refreshVoiceParams();
                    v.start (msg.getNoteNumber(), msg.getFloatVelocity(), currentParams_);
                    break;
                }
            }
        }
        else if (msg.isNoteOff())
        {
            for (auto& v : voices_)
                if (v.isActive())
                    v.stop (true);
        }
    }

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float s = 0.0f;
        for (auto& v : voices_)
            s += v.processSample();
        s *= gain;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.setSample (ch, i, s);
    }
}

juce::AudioProcessorEditor* Rev2TrapAudioProcessor::createEditor()
{
    return new Rev2TrapAudioProcessorEditor (*this);
}

int Rev2TrapAudioProcessor::getNumPrograms()
{
    return rev2trap::presets::kTotalFactoryPrograms;
}

int Rev2TrapAudioProcessor::getCurrentProgram()
{
    return currentProgram_.load();
}

void Rev2TrapAudioProcessor::setCurrentProgram (int index)
{
    applyPreset (index);
}

const juce::String Rev2TrapAudioProcessor::getProgramName (int index)
{
    if (index >= rev2trap::presets::kJz400ProgramOffset)
        return "JZ400 " + juce::String (index - rev2trap::presets::kJz400ProgramOffset + 1);
    return "Rev2 Trap " + juce::String (index + 1);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Rev2TrapAudioProcessor();
}
