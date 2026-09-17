#include "PluginProcessor.h"

#include "Parameters/ParameterIds.h"
#include "PluginEditor.h"
#include "Presets/PresetManager.h"

CharacterFMAudioProcessor::CharacterFMAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_ (*this, nullptr, "CharacterFM", characterfm::createParameterLayout())
{
    masterGainPtr_ = apvts_.getRawParameterValue (characterfm::params::masterGain);
    algorithmPtr_ = apvts_.getRawParameterValue (characterfm::params::algorithm);
    for (int i = 0; i < 6; ++i)
    {
        ratioPtrs_[static_cast<std::size_t> (i)] =
            apvts_.getRawParameterValue (characterfm::params::opRatio (i + 1));
        levelPtrs_[static_cast<std::size_t> (i)] =
            apvts_.getRawParameterValue (characterfm::params::opLevel (i + 1));
    }
}

void CharacterFMAudioProcessor::prepareToPlay (double sampleRate, int)
{
    engine_.reset (sampleRate);
    masterSmoothed_.reset (sampleRate, 0.02);
    syncEngineParams();
}

void CharacterFMAudioProcessor::releaseResources() {}

bool CharacterFMAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
           || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void CharacterFMAudioProcessor::syncEngineParams() noexcept
{
    characterfm::dsp::VoiceParams vp;
    vp.algorithmIndex = algorithmPtr_ != nullptr ? static_cast<int> (*algorithmPtr_) : 0;

    for (int i = 0; i < 6; ++i)
    {
        auto& op = vp.operators[static_cast<std::size_t> (i)];
        op.ratio = ratioPtrs_[static_cast<std::size_t> (i)] != nullptr
                       ? ratioPtrs_[static_cast<std::size_t> (i)]->load()
                       : 1.0f;
        op.level = levelPtrs_[static_cast<std::size_t> (i)] != nullptr
                       ? levelPtrs_[static_cast<std::size_t> (i)]->load()
                       : 0.5f;
        op.attackSec = 0.01f;
        op.decaySec = 0.25f;
        op.sustain = 0.65f;
        op.releaseSec = 0.35f;
    }

    engine_.setGlobalVoiceParams (vp);
}

void CharacterFMAudioProcessor::handleMidi (const juce::MidiBuffer& midi) noexcept
{
    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
            engine_.noteOn (msg.getNoteNumber(), msg.getFloatVelocity());
        else if (msg.isNoteOff())
            engine_.noteOff (msg.getNoteNumber());
        else if (msg.isAllNotesOff() || msg.isAllSoundOff())
            engine_.allNotesOff();
    }
}

void CharacterFMAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    syncEngineParams();

    const float master = masterGainPtr_ != nullptr ? masterGainPtr_->load() : 0.75f;
    masterSmoothed_.setTargetValue (master);

    handleMidi (midi);

    auto* left = buffer.getWritePointer (0);
    auto* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : left;
    const int numSamples = buffer.getNumSamples();

    masterSmoothed_.skip (numSamples - 1);
    const float blockMaster = masterSmoothed_.getNextValue();
    engine_.renderBlock (left, right, numSamples, blockMaster);
}

juce::AudioProcessorEditor* CharacterFMAudioProcessor::createEditor()
{
    return new CharacterFMEditor (*this);
}

void CharacterFMAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts_.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void CharacterFMAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        apvts_.replaceState (juce::ValueTree::fromXml (*xml));
}

bool CharacterFMAudioProcessor::exportPresetJsonToFile (const juce::File& file)
{
    const auto json = characterfm::presets::exportPresetJson (apvts_.copyState(), "User");
    return file.replaceWithText (json);
}

bool CharacterFMAudioProcessor::importPresetJsonFromFile (const juce::File& file)
{
    juce::ValueTree vt;
    if (! characterfm::presets::importPresetJson (file.loadFileAsString(), vt))
        return false;
    apvts_.replaceState (vt);
    return true;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CharacterFMAudioProcessor();
}
