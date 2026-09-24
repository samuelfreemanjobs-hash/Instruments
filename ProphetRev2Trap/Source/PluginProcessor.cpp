#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "DSP/AnalogCircuit.h"
#include "ParameterIds.h"
#include "Presets/PresetManager.h"
#include "Presets/UserPresetStore.h"

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

void ProphetRev2TrapAudioProcessor::SynthVoice::prepareVoice (const juce::dsp::ProcessSpec& spec)
{
    osc1_.prepare (spec);
    osc2_.prepare (spec);
    ladder_.prepare (spec);
    ladder_.setMode (juce::dsp::LadderFilterMode::LPF24);
}

void ProphetRev2TrapAudioProcessor::SynthVoice::glideToNote (int midiNote, float velocity, bool retriggerEnvelopes)
{
    const auto sr = getSampleRate();
    targetFreq_ = juce::MidiMessage::getMidiNoteInHertz (midiNote);
    midiNote_ = midiNote;
    level_ = velocity * 0.35f;

    if (retriggerEnvelopes || ! active_)
    {
        currentFreq_ = targetFreq_;
        osc1_.setFrequency (currentFreq_, true);
        osc2_.setFrequency (currentFreq_ * osc2Ratio_, true);
        osc1_.reset();
        osc2_.reset();

        const auto& rt = owner_.getRuntimeParams();
        ampAdsr_.setSampleRate (sr);
        filtAdsr_.setSampleRate (sr);
        ampAdsr_.setParameters (rt.ampEnv);
        filtAdsr_.setParameters (rt.filtEnv);
        ampAdsr_.noteOn();
        filtAdsr_.noteOn();
        active_ = true;
    }

}

void ProphetRev2TrapAudioProcessor::SynthVoice::startNote (int midiNoteNumber, float velocity,
                                                           juce::SynthesiserSound*, int)
{
    glideToNote (midiNoteNumber, velocity, true);
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
        active_ = false;
    }
}

float ProphetRev2TrapAudioProcessor::SynthVoice::computeCutoffHz (float baseNorm, float filtEnv) noexcept
{
    const auto& rt = owner_.getRuntimeParams();
    const float modNorm = juce::jlimit (0.0f, 1.0f, baseNorm + filtEnv * rt.filtEnvAmt * 0.85f);
    const float baseHz = juce::jmap (modNorm, 80.0f, 14000.0f);
    return prophetrev2::dsp::keyTrackedCutoffHz (baseHz, midiNote_, rt.keyTrack);
}

float ProphetRev2TrapAudioProcessor::SynthVoice::renderOscMix() noexcept
{
    const auto& rt = owner_.getRuntimeParams();
    const float mix = rt.oscMix;
    const int voices = juce::jlimit (1, 3, rt.unisonVoices);
    const float spread = rt.unisonSpreadCents;

    float sum = 0.0f;
    for (int u = 0; u < voices; ++u)
    {
        const float detuneCents = (voices == 1) ? 0.0f
                                                : spread * (static_cast<float> (u) - (voices - 1) * 0.5f);
        const float ratio = std::pow (2.0f, detuneCents / 1200.0f);
        osc2_.setFrequency (currentFreq_ * osc2Ratio_ * ratio, false);

        const float s1 = osc1_.processSample (0.0f) * rt.osc1Level;
        const float s2 = osc2_.processSample (0.0f) * rt.osc2Level;
        sum += s1 * (1.0f - mix) + s2 * mix;
    }
    return sum / static_cast<float> (voices);
}

void ProphetRev2TrapAudioProcessor::SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                                                                 int startSample,
                                                                 int numSamples)
{
    if (! active_)
        return;

    const auto& rt = owner_.getRuntimeParams();

    while (--numSamples >= 0)
    {
        if (std::abs (currentFreq_ - targetFreq_) > 0.05)
            currentFreq_ += (targetFreq_ - currentFreq_) * rt.glideCoeff;
        osc1_.setFrequency (currentFreq_, false);
        osc2_.setFrequency (currentFreq_ * osc2Ratio_, false);

        const float amp = ampAdsr_.getNextSample();
        const float fEnv = filtAdsr_.getNextSample();
        float osc = renderOscMix();
        osc = prophetrev2::dsp::filterInputSat (osc, rt.filterDrive);
        ladder_.setCutoffFrequencyHz (computeCutoffHz (rt.filterCutoffNorm, fEnv));
        ladder_.setResonance (juce::jmap (rt.filterRes, 0.5f, 1.2f));
        float filtered = osc;
        {
            float* ch = &filtered;
            juce::dsp::AudioBlock<float> block (&ch, 1, 1);
            juce::dsp::ProcessContextReplacing<float> ctx (block);
            ladder_.process (ctx);
        }
        filtered = prophetrev2::dsp::softClip (filtered * level_ * amp, rt.circuitDrive);

        for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
            outputBuffer.addSample (ch, startSample, filtered);

        ++startSample;

        if (! ampAdsr_.isActive())
        {
            clearCurrentNote();
            active_ = false;
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
    addFloat (layout, PID::keyTrack, "Key Track", { 0.0f, 1.0f, 0.001f }, 0.45f);
    addFloat (layout, PID::circuitDrive, "Circuit Drive", { 0.0f, 1.0f, 0.001f }, 0.15f);
    addFloat (layout, PID::filterDrive, "Filter Drive", { 0.0f, 1.0f, 0.001f }, 0.2f);
    addFloat (layout, PID::unisonSpread, "Unison Spread", { 0.0f, 40.0f, 0.1f }, 12.0f);
    addFloat (layout, PID::glideMs, "Glide", { 0.0f, 500.0f, 0.1f }, 80.0f);

    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::unisonVoices, 1 },
                                                              "Unison",
                                                              juce::StringArray { "1", "2", "3" },
                                                              0));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::monoMode, 1 }, "Mono", false));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::legatoMode, 1 }, "Legato", true));

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

    int defaultProgram = 0;
    for (int i = 0; i < prophetrev2::presets::PresetManager::getNumFactoryPresets(); ++i)
        if (getProgramName (i) == "Night Circuit")
        {
            defaultProgram = i;
            break;
        }
    applyFactoryPreset (defaultProgram);
    capturePresetBaseline();
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
    return prophetrev2::presets::PresetManager::getNumFactoryPresets();
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
        return juce::String (presets[static_cast<std::size_t> (index)].name);
    }
    return {};
}

void ProphetRev2TrapAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void ProphetRev2TrapAudioProcessor::applySynthParams (const prophetrev2::SynthParams& p)
{
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
    set (PID::keyTrack, p.keyTrack);
    set (PID::circuitDrive, p.circuitDrive);
    set (PID::filterDrive, p.filterDrive);
    set (PID::unisonSpread, p.unisonSpread);
    set (PID::glideMs, p.glideMs);

    if (auto* u = apvts_.getParameter (PID::unisonVoices))
        u->setValueNotifyingHost (juce::jlimit (0.0f, 1.0f, (p.unisonVoices - 1.0f) / 2.0f));
    if (auto* mono = apvts_.getParameter (PID::monoMode))
        mono->setValueNotifyingHost (p.monoMode >= 0.5f ? 1.0f : 0.0f);
    if (auto* leg = apvts_.getParameter (PID::legatoMode))
        leg->setValueNotifyingHost (p.legatoMode >= 0.5f ? 1.0f : 0.0f);

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

void ProphetRev2TrapAudioProcessor::applyFactoryPreset (int index)
{
    const auto& presets = prophetrev2::presets::getFactoryPresets();
    if (index < 0 || index >= static_cast<int> (presets.size()))
        return;

    currentProgram_ = index;
    activeUserPresetName_.clear();
    applySynthParams (presets[static_cast<std::size_t> (index)].params);
    capturePresetBaseline();
}

bool ProphetRev2TrapAudioProcessor::applyUserPreset (const juce::String& name)
{
    juce::ValueTree loaded;
    if (! prophetrev2::presets::UserPresetStore::loadPreset (name, loaded))
        return false;
    if (! loaded.hasType (apvts_.state.getType()))
        return false;

    apvts_.replaceState (loaded);
    activeUserPresetName_ = name;
    currentProgram_ = -1;
    refreshRuntimeParams();
    capturePresetBaseline();
    return true;
}

bool ProphetRev2TrapAudioProcessor::saveUserPreset (const juce::String& name)
{
    const bool ok = prophetrev2::presets::UserPresetStore::savePreset (name, apvts_.copyState());
    if (ok)
    {
        activeUserPresetName_ = name;
        capturePresetBaseline();
    }
    return ok;
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
    runtime_.osc2DetuneRatio = std::pow (2.0f, load (PID::osc2Detune) / 1200.0f);
    runtime_.oscMix = load (PID::oscMix);
    runtime_.filterCutoffNorm = load (PID::filterCutoff);
    runtime_.filterRes = load (PID::filterRes);
    runtime_.filtEnvAmt = load (PID::filtEnvAmt);
    runtime_.keyTrack = load (PID::keyTrack);
    runtime_.circuitDrive = load (PID::circuitDrive);
    runtime_.filterDrive = load (PID::filterDrive);
    runtime_.unisonSpreadCents = load (PID::unisonSpread);
    runtime_.glideMs = load (PID::glideMs);
    runtime_.monoMode = load (PID::monoMode) >= 0.5f;
    runtime_.legatoMode = load (PID::legatoMode) >= 0.5f;
    runtime_.outputGain = juce::Decibels::decibelsToGain (load (PID::outputGain), -100.0f);

    if (auto* uni = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (PID::unisonVoices)))
        runtime_.unisonVoices = uni->getIndex() + 1;
    else
        runtime_.unisonVoices = 1;

    const double sr = getSampleRate() > 0.0 ? getSampleRate() : 44100.0;
    runtime_.glideCoeff = runtime_.glideMs <= 0.0f ? 1.0f
                                                     : static_cast<float> (1.0 - std::exp (-1.0 / (sr * runtime_.glideMs * 0.001)));

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
    synthesiser_.setCurrentPlaybackSampleRate (sampleRate);
    refreshRuntimeParams();

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (juce::jmax (1, samplesPerBlock));
    spec.numChannels = 1;

    for (int i = 0; i < synthesiser_.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<SynthVoice*> (synthesiser_.getVoice (i)))
        {
            voice->prepareVoice (spec);
            voice->setOsc2DetuneRatio (runtime_.osc2DetuneRatio);
        }
}

void ProphetRev2TrapAudioProcessor::releaseResources() {}

bool ProphetRev2TrapAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    return out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo();
}

int ProphetRev2TrapAudioProcessor::countMonoKeysDown() const noexcept
{
    int n = 0;
    for (bool down : monoKeysDown_)
        if (down)
            ++n;
    return n;
}

int ProphetRev2TrapAudioProcessor::highestMonoKeyDown() const noexcept
{
    for (int note = 127; note >= 0; --note)
        if (monoKeysDown_[static_cast<std::size_t> (note)])
            return note;
    return -1;
}

void ProphetRev2TrapAudioProcessor::processMidiMono (juce::MidiBuffer& midi)
{
    juce::MidiBuffer out;

    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            const int note = msg.getNoteNumber();
            const bool wasDown = monoKeysDown_[static_cast<std::size_t> (note)];
            monoKeysDown_[static_cast<std::size_t> (note)] = true;
            const int keysDown = countMonoKeysDown();
            const bool legato = runtime_.legatoMode && (wasDown || keysDown > 1);

            if (! legato)
            {
                synthesiser_.allNotesOff (0, true);
                synthesiser_.noteOn (1, note, msg.getFloatVelocity());
            }
            else if (auto* voice = dynamic_cast<SynthVoice*> (synthesiser_.getVoice (0)))
            {
                voice->glideToNote (note, msg.getFloatVelocity(), false);
            }
        }
        else if (msg.isNoteOff())
        {
            monoKeysDown_[static_cast<std::size_t> (msg.getNoteNumber())] = false;
            if (countMonoKeysDown() == 0)
                synthesiser_.allNotesOff (0, false);
            else if (runtime_.legatoMode)
            {
                const int last = highestMonoKeyDown();
                if (last >= 0)
                    if (auto* voice = dynamic_cast<SynthVoice*> (synthesiser_.getVoice (0)))
                        voice->glideToNote (last, 1.0f, false);
            }
        }
        else
        {
            out.addEvent (msg, metadata.samplePosition);
        }
    }

    midi.swapWith (out);
}

void ProphetRev2TrapAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    refreshRuntimeParams();

    if (runtime_.monoMode)
        processMidiMono (midi);

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
    state.setProperty ("userPreset", activeUserPresetName_, nullptr);
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
        activeUserPresetName_ = vt.getProperty ("userPreset").toString();
        refreshRuntimeParams();
        capturePresetBaseline();
    }
}

void ProphetRev2TrapAudioProcessor::capturePresetBaseline() noexcept
{
    presetBaseline_ = apvts_.copyState();
}

bool ProphetRev2TrapAudioProcessor::isPresetModified() noexcept
{
    if (! presetBaseline_.isValid())
        return false;
    return ! apvts_.copyState().isEquivalentTo (presetBaseline_);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProphetRev2TrapAudioProcessor();
}
