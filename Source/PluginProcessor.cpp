#include "PluginProcessor.h"
#include "PluginEditor.h"

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
    refreshCachedParameters();
    applyPatchesFromParameters();
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

    const float resonance = filterResonancePtr_ != nullptr ? filterResonancePtr_->load() : 0.35f;
    const int couplingIndex = couplingModePtr_ != nullptr ? static_cast<int> (couplingModePtr_->load()) : 0;
    const auto coupling = couplingFromIndex (couplingIndex);

    voicePool_.setCouplingMode (coupling);

    for (std::size_t t = 0; t < jdupgraded::dsp::kTonesPerVoice; ++t)
    {
        patches[t].waveform = &waveLibrary_.getWave (t);
        patches[t].level = levels[t];
        patches[t].filterResonanceNorm = resonance;
        patches[t].phaseModDepth = 0.0f;
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
    if (auto xml = apvts_.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void JDUpgradedAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts_.state.getType()))
            apvts_.replaceState (juce::ValueTree::fromXml (*xml));

    refreshCachedParameters();
}

juce::AudioProcessorEditor* JDUpgradedAudioProcessor::createEditor()
{
    return new JDUpgradedAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JDUpgradedAudioProcessor();
}
