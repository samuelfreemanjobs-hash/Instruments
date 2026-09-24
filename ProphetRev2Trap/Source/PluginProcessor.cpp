#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "ParameterIds.h"
#include "Presets/PresetManager.h"

namespace
{
namespace PID = prophetrev2::ParameterIDs;

constexpr int kNumVoices = 16;

float sawWave (float x)
{
    return (x / juce::MathConstants<float>::twoPi) * 2.0f - 1.0f;
}

float pulseWave (float x)
{
    return x < juce::MathConstants<float>::pi ? 1.0f : -1.0f;
}

void addFloat (juce::AudioProcessorValueTreeState::ParameterLayout& layout,
               const char* id,
               const juce::String& name,
               juce::NormalisableRange<float> range,
               float defaultVal)
{
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, 1 }, name, range, defaultVal));
}
} // namespace

ProphetRev2TrapAudioProcessor::SynthVoice::SynthVoice (ProphetRev2TrapAudioProcessor& owner)
    : owner_ (owner)
{
    osc1_.initialise (sawWave);
    osc2_.initialise (pulseWave);
}

bool ProphetRev2TrapAudioProcessor::SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*> (sound) != nullptr;
}

void ProphetRev2TrapAudioProcessor::SynthVoice::prepareOscillators (const juce::dsp::ProcessSpec& spec)
{
    osc1_.prepare (spec);
    osc2_.prepare (spec);
}

void ProphetRev2TrapAudioProcessor::SynthVoice::startNote (int midiNoteNumber, float velocity,
                                                           juce::SynthesiserSound*, int)
{
    const auto sr = getSampleRate();
    const float freq = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);

    osc1_.setFrequency (freq, true);
    osc2_.setFrequency (freq * osc2Ratio_, true);
    osc1_.reset();
    osc2_.reset();

    level_ = velocity * 0.35f;

    const auto& rt = owner_.getRuntimeParams();
    ampAdsr_.setSampleRate (sr);
    filtAdsr_.setSampleRate (sr);
    ampAdsr_.setParameters (rt.ampEnv);
    filtAdsr_.setParameters (rt.filtEnv);
    ampAdsr_.noteOn();
    filtAdsr_.noteOn();

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sr;
    spec.maximumBlockSize = 512;
    spec.numChannels = 1;
    filter_.prepare (spec);
    filter_.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    filter_.setCutoffFrequency (juce::jmap (rt.filterCutoffNorm, 80.0f, 12000.0f));
    filter_.setResonance (juce::jmap (rt.filterRes, 0.5f, 8.0f));
    filterReady_ = true;
}

void ProphetRev2TrapAudioProcessor::SynthVoice::stopNote (float, bool allowTailOff)
{
    if (allowTailOff)
    {
        ampAdsr_.noteOff();
        filtAdsr_.noteOff();
    }
    else
    {
        ampAdsr_.reset();
        filtAdsr_.reset();
        clearCurrentNote();
        filterReady_ = false;
    }
}

void ProphetRev2TrapAudioProcessor::SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                                                                 int startSample,
                                                                 int numSamples)
{
    if (! filterReady_)
        return;

    const auto& rt = owner_.getRuntimeParams();

    while (--numSamples >= 0)
    {
        const float amp = ampAdsr_.getNextSample();
        const float fEnv = filtAdsr_.getNextSample();
        const float cutoffNorm = juce::jlimit (0.0f, 1.0f, rt.filterCutoffNorm + fEnv * rt.filtEnvAmt * 0.85f);
        filter_.setCutoffFrequency (juce::jmap (cutoffNorm, 80.0f, 14000.0f));

        const float mix = rt.oscMix;
        const float s1 = osc1_.processSample (0.0f) * rt.osc1Level;
        const float s2 = osc2_.processSample (0.0f) * rt.osc2Level;
        const float osc = s1 * (1.0f - mix) + s2 * mix;
        const float filtered = filter_.processSample (0, osc);
        const float sample = filtered * level_ * amp;

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample (ch, startSample, sample);

        ++startSample;

        if (! ampAdsr_.isActive())
        {
            clearCurrentNote();
            filterReady_ = false;
            break;
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout ProphetRev2TrapAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    addFloat (layout, PID::outputGain, "Output", { -24.0f, 12.0f, 0.01f }, -3.0f);
    addFloat (layout, PID::osc1Level, "Osc 1", { 0.0f, 1.0f, 0.001f }, 0.85f);
    addFloat (layout, PID::osc2Level, "Osc 2", { 0.0f, 1.0f, 0.001f }, 0.65f);
    addFloat (layout, PID::osc2Detune, "Osc2 Detune", { -50.0f, 50.0f, 0.01f }, 7.0f);
    addFloat (layout, PID::oscMix, "Osc Mix", { 0.0f, 1.0f, 0.001f }, 0.45f);
    addFloat (layout, PID::filterCutoff, "Cutoff", { 0.0f, 1.0f, 0.0001f }, 0.55f);
    addFloat (layout, PID::filterRes, "Resonance", { 0.0f, 1.0f, 0.0001f }, 0.22f);
    addFloat (layout, PID::filtEnvAmt, "Filt Env Amt", { 0.0f, 1.0f, 0.001f }, 0.5f);

    addFloat (layout, PID::ampAttack, "Amp Attack", { 0.001f, 3.0f, 0.001f, 0.35f }, 0.01f);
    addFloat (layout, PID::ampDecay, "Amp Decay", { 0.001f, 3.0f, 0.001f, 0.35f }, 0.2f);
    addFloat (layout, PID::ampSustain, "Amp Sustain", { 0.0f, 1.0f, 0.001f }, 0.75f);
    addFloat (layout, PID::ampRelease, "Amp Release", { 0.001f, 5.0f, 0.001f, 0.35f }, 0.35f);

    addFloat (layout, PID::filtAttack, "Filt Attack", { 0.001f, 3.0f, 0.001f, 0.35f }, 0.005f);
    addFloat (layout, PID::filtDecay, "Filt Decay", { 0.001f, 3.0f, 0.001f, 0.35f }, 0.35f);
    addFloat (layout, PID::filtSustain, "Filt Sustain", { 0.0f, 1.0f, 0.001f }, 0.25f);
    addFloat (layout, PID::filtRelease, "Filt Release", { 0.001f, 5.0f, 0.001f, 0.35f }, 0.45f);

    return layout;
}

ProphetRev2TrapAudioProcessor::ProphetRev2TrapAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
#endif
      apvts_ (*this, nullptr, "ProphetRev2Trap", createParameterLayout())
{
    for (int i = 0; i < kNumVoices; ++i)
        synthesiser_.addVoice (new SynthVoice (*this));
    synthesiser_.addSound (new SynthSound());

    applyFactoryPreset (0);
}

ProphetRev2TrapAudioProcessor::~ProphetRev2TrapAudioProcessor() = default;

const juce::String ProphetRev2TrapAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ProphetRev2TrapAudioProcessor::acceptsMidi() const { return true; }
bool ProphetRev2TrapAudioProcessor::producesMidi() const { return false; }
bool ProphetRev2TrapAudioProcessor::isMidiEffect() const { return false; }
double ProphetRev2TrapAudioProcessor::getTailLengthSeconds() const { return 3.0; }
bool ProphetRev2TrapAudioProcessor::hasEditor() const { return true; }

int ProphetRev2TrapAudioProcessor::getNumPrograms()
{
    return prophetrev2::presets::PresetManager::getNumPresets();
}

int ProphetRev2TrapAudioProcessor::getCurrentProgram() { return currentProgram_; }

void ProphetRev2TrapAudioProcessor::setCurrentProgram (int index)
{
    applyFactoryPreset (index);
}

const juce::String ProphetRev2TrapAudioProcessor::getProgramName (int index)
{
    const auto& presets = prophetrev2::presets::getFactoryPresets();
    if (index >= 0 && index < static_cast<int> (presets.size()))
    {
        const auto& n = presets[static_cast<std::size_t> (index)].name;
        return juce::String (n.data(), static_cast<int> (n.size()));
    }
    return {};
}

void ProphetRev2TrapAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void ProphetRev2TrapAudioProcessor::applyFactoryPreset (int index)
{
    const auto& presets = prophetrev2::presets::getFactoryPresets();
    if (index < 0 || index >= static_cast<int> (presets.size()))
        return;

    currentProgram_ = index;
    const auto& p = presets[static_cast<std::size_t> (index)].params;

    auto set = [this] (const char* id, float v) {
        if (auto* param = apvts_.getParameter (id))
            param->setValueNotifyingHost (param->convertTo0to1 (v));
    };

    set (PID::osc1Level, p.osc1Level);
    set (PID::osc2Level, p.osc2Level);
    set (PID::osc2Detune, p.osc2DetuneCents);
    set (PID::oscMix, p.oscMix);
    set (PID::filterCutoff, p.filterCutoff);
    set (PID::filterRes, p.filterRes);
    set (PID::filtEnvAmt, p.filtEnvAmt);
    set (PID::ampAttack, p.ampAttack);
    set (PID::ampDecay, p.ampDecay);
    set (PID::ampSustain, p.ampSustain);
    set (PID::ampRelease, p.ampRelease);
    set (PID::filtAttack, p.filtAttack);
    set (PID::filtDecay, p.filtDecay);
    set (PID::filtSustain, p.filtSustain);
    set (PID::filtRelease, p.filtRelease);

    refreshRuntimeParams();
}

void ProphetRev2TrapAudioProcessor::refreshRuntimeParams() noexcept
{
    auto load = [this] (const char* id) -> float {
        if (auto* p = apvts_.getRawParameterValue (id))
            return p->load();
        return 0.0f;
    };

    runtime_.osc1Level = load (PID::osc1Level);
    runtime_.osc2Level = load (PID::osc2Level);
    const float cents = load (PID::osc2Detune);
    runtime_.osc2DetuneRatio = std::pow (2.0f, cents / 1200.0f);
    runtime_.oscMix = load (PID::oscMix);
    runtime_.filterCutoffNorm = load (PID::filterCutoff);
    runtime_.filterRes = load (PID::filterRes);
    runtime_.filtEnvAmt = load (PID::filtEnvAmt);
    runtime_.outputGain = juce::Decibels::decibelsToGain (load (PID::outputGain), -100.0f);

    runtime_.ampEnv.attack = load (PID::ampAttack);
    runtime_.ampEnv.decay = load (PID::ampDecay);
    runtime_.ampEnv.sustain = load (PID::ampSustain);
    runtime_.ampEnv.release = load (PID::ampRelease);

    runtime_.filtEnv.attack = load (PID::filtAttack);
    runtime_.filtEnv.decay = load (PID::filtDecay);
    runtime_.filtEnv.sustain = load (PID::filtSustain);
    runtime_.filtEnv.release = load (PID::filtRelease);

    for (int i = 0; i < synthesiser_.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<SynthVoice*> (synthesiser_.getVoice (i)))
            voice->setOsc2DetuneRatio (runtime_.osc2DetuneRatio);
}

void ProphetRev2TrapAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    synthesiser_.setCurrentPlaybackSampleRate (sampleRate);
    refreshRuntimeParams();

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = 1;

    for (int i = 0; i < synthesiser_.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*> (synthesiser_.getVoice (i)))
        {
            voice->prepareOscillators (spec);
            voice->setOsc2DetuneRatio (runtime_.osc2DetuneRatio);
        }
    }
}

void ProphetRev2TrapAudioProcessor::releaseResources() {}

bool ProphetRev2TrapAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    return out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo();
}

void ProphetRev2TrapAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    refreshRuntimeParams();

    buffer.clear();
    synthesiser_.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());
    buffer.applyGain (runtime_.outputGain);
}

juce::AudioProcessorEditor* ProphetRev2TrapAudioProcessor::createEditor()
{
    return new ProphetRev2TrapAudioProcessorEditor (*this);
}

void ProphetRev2TrapAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    state.setProperty ("currentProgram", currentProgram_, nullptr);
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void ProphetRev2TrapAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml == nullptr)
        return;

    auto vt = juce::ValueTree::fromXml (*xml);
    if (vt.isValid() && vt.hasType (apvts_.state.getType()))
    {
        apvts_.replaceState (vt);
        currentProgram_ = static_cast<int> (vt.getProperty ("currentProgram", 0));
        refreshRuntimeParams();
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProphetRev2TrapAudioProcessor();
}
