#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "Assets/FactoryPatchLibrary.h"
#include "BinaryData.h"

#include <cstdlib>

namespace
{
constexpr const char* kMasterGainId = "masterGain";
constexpr const char* kTone1LevelId = "tone1Level";
constexpr const char* kTone2LevelId = "tone2Level";
constexpr const char* kTone3LevelId = "tone3Level";
constexpr const char* kTone4LevelId = "tone4Level";
constexpr const char* kFilterResonanceId = "filterResonance";
constexpr const char* kCouplingModeId = "couplingMode";
constexpr const char* kGroupADriveId = "groupADrive";
constexpr const char* kGroupBMixId = "groupBMix";
constexpr const char* kTone1WaveId = "tone1Wave";
constexpr const char* kTone2WaveId = "tone2Wave";
constexpr const char* kTone3WaveId = "tone3Wave";
constexpr const char* kTone4WaveId = "tone4Wave";
constexpr const char* kTone1MultisampleId = "tone1Multisample";
constexpr const char* kTone2MultisampleId = "tone2Multisample";
constexpr const char* kTone3MultisampleId = "tone3Multisample";
constexpr const char* kTone4MultisampleId = "tone4Multisample";
constexpr const char* kTone1MuteId = "tone1Mute";
constexpr const char* kTone2MuteId = "tone2Mute";
constexpr const char* kTone3MuteId = "tone3Mute";
constexpr const char* kTone4MuteId = "tone4Mute";

jdupgraded::dsp::ToneCouplingMode couplingFromIndex (int index) noexcept
{
    switch (index)
    {
        case 1: return jdupgraded::dsp::ToneCouplingMode::ringPair01;
        case 2: return jdupgraded::dsp::ToneCouplingMode::ringPair23;
        case 3: return jdupgraded::dsp::ToneCouplingMode::crossModPair01;
        case 4: return jdupgraded::dsp::ToneCouplingMode::crossModPair23;
        case 5: return jdupgraded::dsp::ToneCouplingMode::hardSyncPair01;
        case 6: return jdupgraded::dsp::ToneCouplingMode::hardSyncPair23;
        default: return jdupgraded::dsp::ToneCouplingMode::independent;
    }
}
} // namespace

JDUpgradedAudioProcessor::JDUpgradedAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_ (*this, nullptr, "Parameters", createParameterLayout())
{
    romLoader_.loadFactoryRom (BinaryData::jdupg_cleanroom_rom,
                               BinaryData::jdupg_cleanroom_romSize);

    if (const char* externalRom = std::getenv ("JDUPGRADED_ROM_PATH"))
        romLoader_.loadUserRomFile (externalRom);

    refreshCachedParameters();
    applyFactoryPatch (0);
}

JDUpgradedAudioProcessor::~JDUpgradedAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout JDUpgradedAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kMasterGainId, 1 }, "Master Gain",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.75f));

    for (int i = 1; i <= 4; ++i)
    {
        const juce::String id = "tone" + juce::String (i) + "Level";
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { id, 1 }, "Tone " + juce::String (i) + " Level",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), i == 1 ? 1.0f : 0.0f));
    }

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kFilterResonanceId, 1 }, "Filter Resonance",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.35f));

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { kCouplingModeId, 1 }, "Tone Coupling",
        juce::StringArray { "Independent", "Ring 1-2", "Ring 3-4", "X-Mod 1-2", "X-Mod 3-4", "Sync 1-2", "Sync 3-4" },
        0));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kGroupADriveId, 1 }, "Group A Drive",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { kGroupBMixId, 1 }, "Group B Space",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

    const int maxWave = static_cast<int> (jdupgraded::assets::kCleanroomWaveCount) - 1;
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone1WaveId, 1 }, "Tone 1 Wave", 0, maxWave, 0));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone2WaveId, 1 }, "Tone 2 Wave", 0, maxWave, 16));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone3WaveId, 1 }, "Tone 3 Wave", 0, maxWave, 32));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone4WaveId, 1 }, "Tone 4 Wave", 0, maxWave, 48));

    const int maxMs = static_cast<int> (jdupgraded::assets::kMultisampleSetCount);
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone1MultisampleId, 1 }, "Tone 1 Multisample", 0, maxMs, 0));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone2MultisampleId, 1 }, "Tone 2 Multisample", 0, maxMs, 0));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone3MultisampleId, 1 }, "Tone 3 Multisample", 0, maxMs, 0));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        juce::ParameterID { kTone4MultisampleId, 1 }, "Tone 4 Multisample", 0, maxMs, 0));

    for (int i = 1; i <= 4; ++i)
    {
        const juce::String id = "tone" + juce::String (i) + "Mute";
        params.push_back (std::make_unique<juce::AudioParameterBool> (
            juce::ParameterID { id, 1 }, "Tone " + juce::String (i) + " Mute", false));
    }

    return { params.begin(), params.end() };
}

void JDUpgradedAudioProcessor::refreshCachedParameters() noexcept
{
    masterGainPtr_ = apvts_.getRawParameterValue (kMasterGainId);
    tone1LevelPtr_ = apvts_.getRawParameterValue (kTone1LevelId);
    tone2LevelPtr_ = apvts_.getRawParameterValue (kTone2LevelId);
    tone3LevelPtr_ = apvts_.getRawParameterValue (kTone3LevelId);
    tone4LevelPtr_ = apvts_.getRawParameterValue (kTone4LevelId);
    filterResonancePtr_ = apvts_.getRawParameterValue (kFilterResonanceId);
    couplingModePtr_ = apvts_.getRawParameterValue (kCouplingModeId);
    groupADrivePtr_ = apvts_.getRawParameterValue (kGroupADriveId);
    groupBMixPtr_ = apvts_.getRawParameterValue (kGroupBMixId);
    tone1WavePtr_ = apvts_.getRawParameterValue (kTone1WaveId);
    tone2WavePtr_ = apvts_.getRawParameterValue (kTone2WaveId);
    tone3WavePtr_ = apvts_.getRawParameterValue (kTone3WaveId);
    tone4WavePtr_ = apvts_.getRawParameterValue (kTone4WaveId);
    tone1MultisamplePtr_ = apvts_.getRawParameterValue (kTone1MultisampleId);
    tone2MultisamplePtr_ = apvts_.getRawParameterValue (kTone2MultisampleId);
    tone3MultisamplePtr_ = apvts_.getRawParameterValue (kTone3MultisampleId);
    tone4MultisamplePtr_ = apvts_.getRawParameterValue (kTone4MultisampleId);
    tone1MutePtr_ = apvts_.getRawParameterValue (kTone1MuteId);
    tone2MutePtr_ = apvts_.getRawParameterValue (kTone2MuteId);
    tone3MutePtr_ = apvts_.getRawParameterValue (kTone3MuteId);
    tone4MutePtr_ = apvts_.getRawParameterValue (kTone4MuteId);
}

int JDUpgradedAudioProcessor::getNumPrograms()
{
    return static_cast<int> (jdupgraded::assets::FactoryPatchLibrary::getPatchCount());
}

int JDUpgradedAudioProcessor::getCurrentProgram()
{
    return currentProgram_;
}

void JDUpgradedAudioProcessor::setCurrentProgram (int index)
{
    const auto count = jdupgraded::assets::FactoryPatchLibrary::getPatchCount();
    currentProgram_ = juce::jlimit (0, static_cast<int> (count) - 1, index);
    applyFactoryPatch (currentProgram_);
}

const juce::String JDUpgradedAudioProcessor::getProgramName (int index)
{
    return juce::String (jdupgraded::assets::FactoryPatchLibrary::getPatch (static_cast<std::size_t> (index)).name);
}

void JDUpgradedAudioProcessor::setApvtsFloat (const char* paramId, float value)
{
    if (auto* param = apvts_.getParameter (paramId))
        param->setValueNotifyingHost (param->convertTo0to1 (value));
}

void JDUpgradedAudioProcessor::setApvtsInt (const char* paramId, int value)
{
    if (auto* param = apvts_.getParameter (paramId))
        param->setValueNotifyingHost (param->convertTo0to1 (static_cast<float> (value)));
}

void JDUpgradedAudioProcessor::setApvtsChoice (const char* paramId, int index)
{
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (paramId)))
        param->setValueNotifyingHost (param->convertTo0to1 (static_cast<float> (index)));
}

void JDUpgradedAudioProcessor::setApvtsBool (const char* paramId, bool value)
{
    if (auto* param = apvts_.getParameter (paramId))
        param->setValueNotifyingHost (value ? 1.0f : 0.0f);
}

void JDUpgradedAudioProcessor::applyFactoryPatch (int index)
{
    const auto& patch = jdupgraded::assets::FactoryPatchLibrary::getPatch (static_cast<std::size_t> (index));

    setApvtsFloat (kMasterGainId, patch.masterGain);
    setApvtsFloat (kGroupADriveId, patch.groupADrive);
    setApvtsFloat (kGroupBMixId, patch.groupBMix);

    int couplingIndex = 0;
    switch (patch.coupling)
    {
        case jdupgraded::dsp::ToneCouplingMode::ringPair01: couplingIndex = 1; break;
        case jdupgraded::dsp::ToneCouplingMode::ringPair23: couplingIndex = 2; break;
        case jdupgraded::dsp::ToneCouplingMode::crossModPair01: couplingIndex = 3; break;
        case jdupgraded::dsp::ToneCouplingMode::crossModPair23: couplingIndex = 4; break;
        case jdupgraded::dsp::ToneCouplingMode::hardSyncPair01: couplingIndex = 5; break;
        case jdupgraded::dsp::ToneCouplingMode::hardSyncPair23: couplingIndex = 6; break;
        default: break;
    }
    setApvtsChoice (kCouplingModeId, couplingIndex);

    const char* levelIds[4] = { kTone1LevelId, kTone2LevelId, kTone3LevelId, kTone4LevelId };
    const char* waveIds[4] = { kTone1WaveId, kTone2WaveId, kTone3WaveId, kTone4WaveId };
    const char* msIds[4] = { kTone1MultisampleId, kTone2MultisampleId, kTone3MultisampleId, kTone4MultisampleId };
    const char* muteIds[4] = { kTone1MuteId, kTone2MuteId, kTone3MuteId, kTone4MuteId };

    for (int t = 0; t < 4; ++t)
    {
        const auto& tone = patch.tones[static_cast<std::size_t> (t)];
        toneCoarseSemis_[static_cast<std::size_t> (t)] = tone.coarseSemis;
        toneFilterCutoff_[static_cast<std::size_t> (t)] = tone.filterCutoffNorm;
        toneFilterResonance_[static_cast<std::size_t> (t)] = tone.filterResonanceNorm;

        setApvtsFloat (levelIds[t], tone.level);
        setApvtsInt (waveIds[t], static_cast<int> (tone.waveIndex));
        setApvtsInt (msIds[t], static_cast<int> (tone.multisampleSetId));
        setApvtsBool (muteIds[t], tone.level < 0.001f);
    }

    setApvtsFloat (kFilterResonanceId, patch.tones[0].filterResonanceNorm);
    refreshCachedParameters();
    applyPatchesFromParameters();
}

void JDUpgradedAudioProcessor::applyPatchesFromParameters() noexcept
{
    jdupgraded::dsp::TonePatchSet patches{};

    const float levels[4] = {
        tone1LevelPtr_ != nullptr ? tone1LevelPtr_->load() : 1.0f,
        tone2LevelPtr_ != nullptr ? tone2LevelPtr_->load() : 0.0f,
        tone3LevelPtr_ != nullptr ? tone3LevelPtr_->load() : 0.0f,
        tone4LevelPtr_ != nullptr ? tone4LevelPtr_->load() : 0.0f,
    };

    const bool mutes[4] = {
        tone1MutePtr_ != nullptr && tone1MutePtr_->load() >= 0.5f,
        tone2MutePtr_ != nullptr && tone2MutePtr_->load() >= 0.5f,
        tone3MutePtr_ != nullptr && tone3MutePtr_->load() >= 0.5f,
        tone4MutePtr_ != nullptr && tone4MutePtr_->load() >= 0.5f,
    };

    int couplingIndex = 0;
    if (auto* couplingParam = dynamic_cast<juce::AudioParameterChoice*> (apvts_.getParameter (kCouplingModeId)))
        couplingIndex = couplingParam->getIndex();
    const auto coupling = couplingFromIndex (couplingIndex);

    voicePool_.setCouplingMode (coupling);

    const std::size_t waveIndices[4] = {
        tone1WavePtr_ != nullptr ? static_cast<std::size_t> (tone1WavePtr_->load()) : 0,
        tone2WavePtr_ != nullptr ? static_cast<std::size_t> (tone2WavePtr_->load()) : 16,
        tone3WavePtr_ != nullptr ? static_cast<std::size_t> (tone3WavePtr_->load()) : 32,
        tone4WavePtr_ != nullptr ? static_cast<std::size_t> (tone4WavePtr_->load()) : 48,
    };

    const std::uint16_t multisampleIds[4] = {
        tone1MultisamplePtr_ != nullptr ? static_cast<std::uint16_t> (tone1MultisamplePtr_->load()) : 0,
        tone2MultisamplePtr_ != nullptr ? static_cast<std::uint16_t> (tone2MultisamplePtr_->load()) : 0,
        tone3MultisamplePtr_ != nullptr ? static_cast<std::uint16_t> (tone3MultisamplePtr_->load()) : 0,
        tone4MultisamplePtr_ != nullptr ? static_cast<std::uint16_t> (tone4MultisamplePtr_->load()) : 0,
    };

    const jdupgraded::assets::RomBank* bank = romLoader_.getBank().isLoaded() ? &romLoader_.getBank() : nullptr;

    for (std::size_t t = 0; t < jdupgraded::dsp::kTonesPerVoice; ++t)
    {
        patches[t].romBank = bank;
        patches[t].multisampleSetId = multisampleIds[t];
        patches[t].waveIndex = static_cast<std::uint16_t> (waveIndices[t]);
        patches[t].coarseSemis = toneCoarseSemis_[t];
        patches[t].filterCutoffNorm = toneFilterCutoff_[t];
        patches[t].filterResonanceNorm = toneFilterResonance_[t];
        patches[t].level = mutes[t] ? 0.0f : levels[t];
        patches[t].phaseModDepth = 0.0f;
        patches[t].waveform = bank == nullptr ? &fallbackWaves_.getWave (waveIndices[t]) : nullptr;
    }

    if (coupling == jdupgraded::dsp::ToneCouplingMode::crossModPair01)
        patches[1].phaseModDepth = 0.4f;
    if (coupling == jdupgraded::dsp::ToneCouplingMode::crossModPair23)
        patches[3].phaseModDepth = 0.4f;

    voicePool_.setDefaultPatches (patches);
}

void JDUpgradedAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    voicePool_.prepare (sampleRate, static_cast<std::size_t> (samplesPerBlock));
    groupB_.prepare (sampleRate);
    applyPatchesFromParameters();
}

void JDUpgradedAudioProcessor::releaseResources() {}

bool JDUpgradedAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void JDUpgradedAudioProcessor::handleMidi (const juce::MidiBuffer& midi) noexcept
{
    for (const auto metadata : midi)
    {
        const auto message = metadata.getMessage();
        if (message.isNoteOn())
        {
            voicePool_.noteOn (static_cast<std::uint8_t> (message.getNoteNumber()),
                               static_cast<std::uint8_t> (message.getVelocity()));
        }
        else if (message.isNoteOff())
        {
            voicePool_.noteOff (static_cast<std::uint8_t> (message.getNoteNumber()));
        }
        else if (message.isAllNotesOff() || message.isAllSoundOff())
        {
            voicePool_.allNotesOff();
        }
    }
}

void JDUpgradedAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    applyPatchesFromParameters();

    const auto totalNumInputChannels = getTotalNumInputChannels();
    for (int i = totalNumInputChannels; i < getTotalNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    handleMidi (midiMessages);

    const auto numSamples = static_cast<std::size_t> (buffer.getNumSamples());
    auto* left = buffer.getWritePointer (0);
    float* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : nullptr;

    voicePool_.render (outputScratch_.data(), numSamples);

    const float master = masterGainPtr_ != nullptr ? masterGainPtr_->load() : 0.75f;
    const float groupADrive = groupADrivePtr_ != nullptr ? groupADrivePtr_->load() : 0.0f;
    const float groupBMix = groupBMixPtr_ != nullptr ? groupBMixPtr_->load() : 0.0f;

    groupA_.setDrive (groupADrive);
    groupB_.setMix (groupBMix);

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float l = outputScratch_[static_cast<std::size_t> (i)] * master;
        float r = l;

        l = groupA_.process (l);
        r = groupA_.process (r);

        groupB_.processStereo (l, r);

        left[i] = l;
        if (right != nullptr)
            right[i] = r;
    }
}

void JDUpgradedAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    state.setProperty ("currentProgram", currentProgram_, nullptr);
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void JDUpgradedAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
    {
        if (xml->hasTagName (apvts_.state.getType()))
        {
            auto state = juce::ValueTree::fromXml (*xml);
            apvts_.replaceState (state);
            currentProgram_ = static_cast<int> (state.getProperty ("currentProgram", 0));
        }
    }

    refreshCachedParameters();
    applyPatchesFromParameters();
}

juce::AudioProcessorEditor* JDUpgradedAudioProcessor::createEditor()
{
    return new JDUpgradedAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JDUpgradedAudioProcessor();
}
