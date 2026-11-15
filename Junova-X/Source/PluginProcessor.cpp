#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Golden/GoldenScenarios.h"
#include "Presets/PresetParams.h"

namespace
{
using PID = junovax::ParameterIDs;

void addFloat (juce::AudioProcessorValueTreeState::ParameterLayout& layout,
               const char* id,
               const juce::String& name,
               juce::NormalisableRange<float> range,
               float defaultValue)
{
    layout.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, 1 }, name, range, defaultValue));
}
} // namespace

juce::AudioProcessorValueTreeState::ParameterLayout JunovaXAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    addFloat (layout, PID::masterGain, "Master", { -24.0f, 12.0f, 0.01f }, 0.0f);

    addFloat (layout, PID::ampAttack, "Amp Attack", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.01f);
    addFloat (layout, PID::ampDecay, "Amp Decay", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.2f);
    addFloat (layout, PID::ampSustain, "Amp Sustain", { 0.0f, 1.0f, 0.001f }, 0.75f);
    addFloat (layout, PID::ampRelease, "Amp Release", { 0.001f, 5.0f, 0.001f, 0.35f }, 0.4f);

    addFloat (layout, PID::filtAttack, "Filt Attack", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.005f);
    addFloat (layout, PID::filtDecay, "Filt Decay", { 0.001f, 2.0f, 0.001f, 0.35f }, 0.35f);
    addFloat (layout, PID::filtSustain, "Filt Sustain", { 0.0f, 1.0f, 0.001f }, 0.4f);
    addFloat (layout, PID::filtRelease, "Filt Release", { 0.001f, 5.0f, 0.001f, 0.35f }, 0.5f);

    addFloat (layout, PID::filterCutoff, "Cutoff", { 0.0f, 1.0f, 0.0001f }, 0.65f);
    addFloat (layout, PID::filterRes, "Resonance", { 0.0f, 1.0f, 0.0001f }, 0.15f);
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::hpfMode, 1 },
                                                            "HPF",
                                                            juce::StringArray { "Off", "80 Hz", "120 Hz", "220 Hz" },
                                                            0));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::hpfEnabled, 1 }, "HPF Legacy", false));

    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::chorusMode, 1 },
                                                              "Chorus",
                                                              juce::StringArray { "Off", "I", "II", "I+II" },
                                                              0));
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::voiceMode, 1 },
                                                              "Voice",
                                                              juce::StringArray { "Poly 1", "Poly 2", "Unison", "Mono", "Juno 6" },
                                                              0));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::chorusHiss, 1 }, "Chorus Hiss", true));

    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::lfoWaveform, 1 },
                                                              "LFO Wave",
                                                              juce::StringArray { "Tri", "Sqr", "S&H" },
                                                              0));
    addFloat (layout, PID::lfoRate, "LFO Rate", { 0.01f, 20.0f, 0.001f, 0.4f }, 4.2f);
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::lfoBpmSync, 1 }, "LFO BPM Sync", false));
    addFloat (layout, PID::lfoDelay, "LFO Delay", { 0.0f, 2.0f, 0.001f }, 0.15f);
    addFloat (layout, PID::glide, "Glide", { 0.0f, 500.0f, 0.1f }, 12.0f);

    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::dcoOctave, 1 },
                                                              "DCO Octave",
                                                              juce::StringArray { "16'", "8'", "4'", "2'" },
                                                              1));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::dcoPulseOn, 1 }, "Pulse", true));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::dcoSawOn, 1 }, "Saw", true));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::dcoSubOn, 1 }, "Sub", true));
    addFloat (layout, PID::dcoLfoMod, "DCO LFO Mod", { 0.0f, 100.0f, 0.1f }, 30.0f);
    addFloat (layout, PID::dcoPwm, "PWM", { 0.0f, 100.0f, 0.1f }, 65.0f);
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::pwmSourceLfo, 1 }, "PWM LFO", true));
    addFloat (layout, PID::dcoSubLvl, "Sub Level", { 0.0f, 100.0f, 0.1f }, 80.0f);
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::dcoSubOct, 1 },
                                                              "Sub Oct",
                                                              juce::StringArray { "-1 Oct", "-2 Oct" },
                                                              0));
    addFloat (layout, PID::dcoNoise, "Noise", { 0.0f, 100.0f, 0.1f }, 10.0f);

    addFloat (layout, PID::hpfCutoff, "HPF Cutoff Legacy", { 0.0f, 1.0f, 0.0001f }, 0.2f);

    addFloat (layout, PID::vcfEnv, "VCF Env", { 0.0f, 100.0f, 0.1f }, 55.0f);
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::vcfEnvInv, 1 }, "VCF Env Inv", false));
    addFloat (layout, PID::vcfDrive, "VCF Drive", { 0.0f, 100.0f, 0.1f }, 35.0f);
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::vcaGateMode, 1 }, "VCA Gate", false));
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::envVintageCurve, 1 }, "Vintage Env", true));
    addFloat (layout, PID::vcfLfo, "VCF LFO", { 0.0f, 100.0f, 0.1f }, 33.0f);
    addFloat (layout, PID::vcfKey, "VCF Key", { 0.0f, 100.0f, 0.1f }, 85.0f);

    addFloat (layout, PID::drift, "Drift", { 0.0f, 100.0f, 0.1f }, 14.0f);
    addFloat (layout, PID::detune, "Detune", { -50.0f, 50.0f, 0.01f }, 6.0f);
    addFloat (layout, PID::width, "Width", { 0.0f, 200.0f, 0.1f }, 100.0f);

    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::arpEnabled, 1 }, "Arp On", false));
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::arpDirection, 1 },
                                                              "Arp Dir",
                                                              juce::StringArray { "Up", "Down", "U/D", "Rnd" },
                                                              0));
    addFloat (layout, PID::arpRange, "Arp Range", { 1.0f, 4.0f, 1.0f }, 2.0f);
    addFloat (layout, PID::arpRate, "Arp Rate", { 0.0f, 1.0f, 0.001f }, 0.25f);
    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::arpLatch, 1 }, "Arp Latch", false));

    addFloat (layout, PID::editorZoom, "Editor Zoom", { 0.75f, 1.5f, 0.01f }, 1.0f);
    layout.add (std::make_unique<juce::AudioParameterChoice> (juce::ParameterID { PID::oversampleMode, 1 },
                                                              "Oversample",
                                                              juce::StringArray { "1x", "2x", "4x" },
                                                              1));

    layout.add (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { PID::diagTestTone, 1 }, "Diag Test Tone", false));
    addFloat (layout, PID::diagToneFreq, "Diag Freq", { 55.0f, 880.0f, 0.01f, 0.5f }, 440.0f);

    return layout;
}

JunovaXAudioProcessor::JunovaXAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
#endif
      apvts_ (*this, nullptr, "JunovaX", createParameterLayout())
{
    applyFactoryPreset (0);
}

JunovaXAudioProcessor::~JunovaXAudioProcessor() = default;

const juce::String JunovaXAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JunovaXAudioProcessor::acceptsMidi() const { return true; }
bool JunovaXAudioProcessor::producesMidi() const { return false; }
bool JunovaXAudioProcessor::isMidiEffect() const { return false; }
double JunovaXAudioProcessor::getTailLengthSeconds() const { return 0.5; }
bool JunovaXAudioProcessor::hasEditor() const { return true; }

int JunovaXAudioProcessor::getNumPrograms()
{
    return static_cast<int> (junovax::presets::getFactoryPresets().size());
}

int JunovaXAudioProcessor::getCurrentProgram() { return currentProgram_; }

void JunovaXAudioProcessor::setCurrentProgram (int index)
{
    applyFactoryPreset (index);
}

const juce::String JunovaXAudioProcessor::getProgramName (int index)
{
    const auto& presets = junovax::presets::getFactoryPresets();
    if (index >= 0 && index < static_cast<int> (presets.size()))
        return juce::String (presets[static_cast<std::size_t> (index)].name.data());
    return {};
}

void JunovaXAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

bool JunovaXAudioProcessor::applyGoldenScenario (const char* scenarioId) noexcept
{
    if (! junovax::golden::applyScenario (*this, scenarioId))
        return false;
    pushParamsToEngine();
    return true;
}

void JunovaXAudioProcessor::applyFactoryPreset (int index)
{
    const auto& presets = junovax::presets::getFactoryPresets();
    if (index < 0 || index >= static_cast<int> (presets.size()))
        return;

    currentProgram_ = index;
    const auto& p = presets[static_cast<std::size_t> (index)].params;

    auto setF = [this] (const char* id, float v)
    {
        if (auto* param = apvts_.getParameter (id))
            param->setValueNotifyingHost (param->convertTo0to1 (v));
    };
    auto setB = [this] (const char* id, bool v)
    {
        if (auto* param = apvts_.getParameter (id))
            param->setValueNotifyingHost (v ? 1.0f : 0.0f);
    };
    auto setC = [this] (const char* id, int choiceIndex, int numChoices)
    {
        if (auto* param = apvts_.getParameter (id))
            param->setValueNotifyingHost (static_cast<float> (choiceIndex) / static_cast<float> (numChoices - 1));
    };

    setF (PID::masterGain, p.masterGain);
    setF (PID::ampAttack, p.ampAttack);
    setF (PID::ampDecay, p.ampDecay);
    setF (PID::ampSustain, p.ampSustain);
    setF (PID::ampRelease, p.ampRelease);
    setF (PID::filtAttack, p.filtAttack);
    setF (PID::filtDecay, p.filtDecay);
    setF (PID::filtSustain, p.filtSustain);
    setF (PID::filtRelease, p.filtRelease);
    setF (PID::filterCutoff, p.filterCutoff);
    setF (PID::filterRes, p.filterRes);
    int hpfMode = p.hpfMode;
    if (hpfMode <= 0 && p.hpfEnabled)
        hpfMode = p.hpfCutoff < 0.18f ? 1 : (p.hpfCutoff < 0.35f ? 2 : 3);
    setC (PID::hpfMode, hpfMode, 4);
    setB (PID::hpfEnabled, p.hpfEnabled);
    setF (PID::hpfCutoff, p.hpfCutoff);
    setB (PID::chorusHiss, p.chorusHiss);
    setC (PID::chorusMode, p.chorusMode, 4);
    setC (PID::lfoWaveform, p.lfoWaveform, 3);
    setB (PID::lfoBpmSync, p.lfoBpmSync);
    setC (PID::dcoOctave, p.dcoOctave, 4);
    setB (PID::dcoPulseOn, p.dcoPulseOn);
    setB (PID::dcoSawOn, p.dcoSawOn);
    setB (PID::dcoSubOn, p.dcoSubOn);
    setB (PID::pwmSourceLfo, p.pwmSourceLfo);
    setC (PID::dcoSubOct, p.dcoSubOct, 2);
    setB (PID::vcfEnvInv, p.vcfEnvInv);
    setF (PID::vcfDrive, p.vcfDrive);
    setB (PID::vcaGateMode, p.vcaGateMode);
    setB (PID::envVintageCurve, p.envVintageCurve);
    setB (PID::arpEnabled, p.arpEnabled);
    setC (PID::arpDirection, p.arpDirection, 4);
    setC (PID::voiceMode, p.voiceMode, 5);
    setF (PID::lfoRate, p.lfoRate);
    setF (PID::lfoDelay, p.lfoDelay);
    setF (PID::glide, p.glide);
    setF (PID::dcoLfoMod, p.dcoLfoMod);
    setF (PID::dcoPwm, p.dcoPwm);
    setF (PID::dcoSubLvl, p.dcoSubLvl);
    setF (PID::dcoNoise, p.dcoNoise);
    setF (PID::vcfEnv, p.vcfEnv);
    setF (PID::vcfLfo, p.vcfLfo);
    setF (PID::vcfKey, p.vcfKey);
    setF (PID::drift, p.drift);
    setF (PID::detune, p.detune);
    setF (PID::width, p.width);
    setF (PID::arpRange, p.arpRange);
    setF (PID::arpRate, p.arpRate);
    setB (PID::arpLatch, p.arpLatch);

    pushParamsToEngine();
}

void JunovaXAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    lastSampleRate_ = sampleRate;
    arpeggiator_.prepare (sampleRate);
    engine_.prepare (sampleRate, samplesPerBlock);
    pushParamsToEngine();
}

void JunovaXAudioProcessor::releaseResources() {}

bool JunovaXAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void JunovaXAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    junovax::dsp::ArpHostContext host;
    host.bpm = 120.0;
    host.ppqValid = false;
    if (auto* head = getPlayHead())
    {
        if (auto pos = head->getPosition())
        {
            host.bpm = pos->getBpm().orFallback (120.0);
            if (auto ppq = pos->getPpqPosition())
            {
                host.ppqPosition = *ppq;
                host.ppqValid = true;
            }
        }
    }
    displayBpm_.store (static_cast<float> (host.bpm), std::memory_order_relaxed);

    keyboardState_.processNextMidiBuffer (midi, 0, buffer.getNumSamples(), true);
    applyTransposeToMidiBuffer (midi, midiTranspose_.load (std::memory_order_relaxed));

    auto params = readParamsFromApvts();
    params.hostBpm = static_cast<float> (host.bpm);
    engine_.setParams (params);

    int arpDir = 0;
    if (auto* dir = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (PID::arpDirection)))
        arpDir = dir->getIndex();

    juce::MidiBuffer arpMidi;
    arpeggiator_.process (midi,
                           arpMidi,
                           params.arpRate,
                           params.arpRange,
                           params.arpLatch,
                           params.arpEnabled,
                           arpDir,
                           host,
                           buffer.getNumSamples());
    engine_.render (buffer, arpMidi);
    outputPeak_.store (engine_.readAndResetPeak(), std::memory_order_relaxed);
    activeVoiceMask_.store (engine_.getActiveVoiceMask(), std::memory_order_relaxed);

    if (buffer.getNumChannels() > 0)
    {
        const auto* mono = buffer.getReadPointer (0);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            scopeFifo_.pushSample (mono[i]);
    }
}

void JunovaXAudioProcessor::panicAllNotes()
{
    arpeggiator_.reset();
    engine_.panic();
    if (auto* tone = apvts_.getParameter (PID::diagTestTone))
        tone->setValueNotifyingHost (0.0f);
}

junovax::dsp::RuntimeParams JunovaXAudioProcessor::readParamsFromApvts() const noexcept
{
    junovax::dsp::RuntimeParams p;
    auto f = [this] (const char* id) -> float
    {
        if (auto* param = apvts_.getRawParameterValue (id))
            return param->load();
        return 0.0f;
    };

    p.masterGainDb = f (PID::masterGain);
    p.ampAttack = f (PID::ampAttack);
    p.ampDecay = f (PID::ampDecay);
    p.ampSustain = f (PID::ampSustain);
    p.ampRelease = f (PID::ampRelease);
    p.filtAttack = f (PID::filtAttack);
    p.filtDecay = f (PID::filtDecay);
    p.filtSustain = f (PID::filtSustain);
    p.filtRelease = f (PID::filtRelease);
    p.filterCutoff = f (PID::filterCutoff);
    p.filterRes = f (PID::filterRes);
    if (auto* hpf = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (PID::hpfMode)))
        p.hpfMode = hpf->getIndex();
    if (auto* chorus = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (PID::chorusMode)))
        p.chorusMode = chorus->getIndex();
    p.chorusHiss = f (PID::chorusHiss) > 0.5f;
    if (auto* voice = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (PID::voiceMode)))
        p.voiceMode = voice->getIndex();
    if (auto* lfoW = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (PID::lfoWaveform)))
        p.lfoWaveform = lfoW->getIndex();
    p.lfoRate = f (PID::lfoRate);
    p.lfoBpmSync = f (PID::lfoBpmSync) > 0.5f;
    if (auto* oct = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (PID::dcoOctave)))
        p.dcoOctave = oct->getIndex();
    p.dcoPulseOn = f (PID::dcoPulseOn) > 0.5f;
    p.dcoSawOn = f (PID::dcoSawOn) > 0.5f;
    p.dcoSubOn = f (PID::dcoSubOn) > 0.5f;
    p.pwmSourceLfo = f (PID::pwmSourceLfo) > 0.5f;
    if (auto* subOct = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (PID::dcoSubOct)))
        p.dcoSubOct = subOct->getIndex();
    p.lfoDelay = f (PID::lfoDelay);
    p.glideMs = f (PID::glide);
    p.dcoLfoMod = f (PID::dcoLfoMod);
    p.dcoPwm = f (PID::dcoPwm);
    p.dcoSubLvl = f (PID::dcoSubLvl);
    p.dcoNoise = f (PID::dcoNoise);
    p.vcfEnv = f (PID::vcfEnv);
    p.vcfLfo = f (PID::vcfLfo);
    p.vcfKey = f (PID::vcfKey);
    p.vcfEnvInv = f (PID::vcfEnvInv) > 0.5f;
    p.vcfDrive = f (PID::vcfDrive);
    p.vcaGateMode = f (PID::vcaGateMode) > 0.5f;
    p.envVintageCurve = f (PID::envVintageCurve) > 0.5f;
    p.drift = f (PID::drift);
    p.detune = f (PID::detune);
    p.width = f (PID::width);
    p.arpEnabled = f (PID::arpEnabled) > 0.5f;
    if (auto* dir = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (PID::arpDirection)))
        p.arpDirection = dir->getIndex();
    p.arpRange = f (PID::arpRange);
    p.arpRate = f (PID::arpRate);
    p.arpLatch = f (PID::arpLatch) > 0.5f;
    p.diagTestTone = f (PID::diagTestTone) > 0.5f;
    p.diagToneFreqHz = f (PID::diagToneFreq);
    if (auto* os = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (PID::oversampleMode)))
        p.oversampleMode = os->getIndex();
    return p;
}

void JunovaXAudioProcessor::pushParamsToEngine() noexcept
{
    auto p = readParamsFromApvts();
    p.hostBpm = displayBpm_.load (std::memory_order_relaxed);
    engine_.setParams (p);
}

void JunovaXAudioProcessor::initToDefaultPatch()
{
    applyFactoryPreset (0);
}

void JunovaXAudioProcessor::saveUserPresetToFile()
{
    auto chooser = std::make_shared<juce::FileChooser> ("Save Junova-X preset", juce::File(), "*.junovax");
    chooser->launchAsync (juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
                          [this, chooser] (const juce::FileChooser& fc)
                          {
                              const auto file = fc.getResult();
                              if (file == juce::File())
                                  return;
                              juce::MemoryBlock data;
                              getStateInformation (data);
                              file.replaceWithData (data.getData(), data.getSize());
                          });
}

void JunovaXAudioProcessor::copyPatchA() { patchA_ = apvts_.copyState().createCopy(); }

void JunovaXAudioProcessor::pastePatchA()
{
    if (! patchA_.isValid())
        return;
    apvts_.replaceState (patchA_.createCopy());
    pushParamsToEngine();
}

void JunovaXAudioProcessor::toggleAbSlot()
{
    copyPatchA();
    pastePatchA();
}

void JunovaXAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    state.setProperty ("currentProgram", currentProgram_, nullptr);
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void JunovaXAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
    {
        if (xml->hasTagName (apvts_.state.getType()))
        {
            auto tree = juce::ValueTree::fromXml (*xml);
            apvts_.replaceState (tree);
            const int prog = static_cast<int> (tree.getProperty ("currentProgram", 0));
            currentProgram_ = prog;
            pushParamsToEngine();
        }
    }
}

void JunovaXAudioProcessor::setMidiTransposeSemitones (int semitones) noexcept
{
    const int clamped = juce::jlimit (-48, 48, semitones);
    midiTranspose_.store (clamped, std::memory_order_relaxed);
}

void JunovaXAudioProcessor::applyTransposeToMidiBuffer (juce::MidiBuffer& midi, int semitones) noexcept
{
    if (semitones == 0)
        return;

    juce::MidiBuffer out;
    for (const auto metadata : midi)
    {
        auto msg = metadata.getMessage();
        if (msg.isNoteOnOrOff())
        {
            const int note = juce::jlimit (0, 127, msg.getNoteNumber() + semitones);
            if (msg.isNoteOn())
                msg = juce::MidiMessage::noteOn (msg.getChannel(), note, msg.getVelocity());
            else
                msg = juce::MidiMessage::noteOff (msg.getChannel(), note);
        }
        out.addEvent (msg, metadata.samplePosition);
    }
    midi.swapWith (out);
}

juce::AudioProcessorEditor* JunovaXAudioProcessor::createEditor()
{
    return new JunovaXAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JunovaXAudioProcessor();
}
