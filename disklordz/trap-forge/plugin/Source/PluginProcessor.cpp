#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
float applyOutputCeiling (float sample, float ceilingDb)
{
    const float gain = juce::Decibels::decibelsToGain (ceilingDb, -100.0f);
    return juce::jlimit (-1.0f, 1.0f, sample * gain);
}
} // namespace

TrapForgeAudioProcessor::TrapForgeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true))
#endif
{
    factoryPresets_ = trapforge::loadFactoryPresets();
    refreshActivePreset();
}

TrapForgeAudioProcessor::~TrapForgeAudioProcessor() = default;

const juce::String TrapForgeAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TrapForgeAudioProcessor::acceptsMidi() const { return true; }
bool TrapForgeAudioProcessor::producesMidi() const { return false; }
bool TrapForgeAudioProcessor::isMidiEffect() const { return false; }
double TrapForgeAudioProcessor::getTailLengthSeconds() const { return 2.5; }
bool TrapForgeAudioProcessor::hasEditor() const { return true; }

int TrapForgeAudioProcessor::getNumPrograms()
{
    return factoryPresets_.size();
}

int TrapForgeAudioProcessor::getCurrentProgram()
{
    return currentProgram_;
}

void TrapForgeAudioProcessor::refreshActivePreset()
{
    if (factoryPresets_.isEmpty())
    {
        activePreset_ = trapforge::TrapForgePreset();
        return;
    }
    currentProgram_ = juce::jlimit (0, factoryPresets_.size() - 1, currentProgram_);
    activePreset_ = factoryPresets_.getReference (currentProgram_);
}

void TrapForgeAudioProcessor::setCurrentProgram (int index)
{
    if (index < 0 || index >= factoryPresets_.size())
        return;
    currentProgram_ = index;
    refreshActivePreset();
}

const juce::String TrapForgeAudioProcessor::getProgramName (int index)
{
    if (index >= 0 && index < factoryPresets_.size())
        return factoryPresets_.getReference (index).name;
    return {};
}

void TrapForgeAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void TrapForgeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    sampleRate_ = sampleRate;
    const juce::ScopedLock lock (voiceLock_);
    voices_.clear();
}

void TrapForgeAudioProcessor::releaseResources()
{
    const juce::ScopedLock lock (voiceLock_);
    voices_.clear();
}

bool TrapForgeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& main = layouts.getMainOutputChannelSet();
    return main == juce::AudioChannelSet::mono() || main == juce::AudioChannelSet::stereo();
}

void TrapForgeAudioProcessor::triggerVoice (float velocity)
{
    Voice voice;
    voice.mono = trapforge::renderDrumMono (activePreset_, sampleRate_, velocity);
    if (voice.mono.empty())
        return;

    const juce::ScopedLock lock (voiceLock_);
    voices_.add (std::move (voice));
}

void TrapForgeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;

    for (const auto metadata : midi)
    {
        const auto message = metadata.getMessage();
        if (message.isNoteOn())
            triggerVoice (message.getFloatVelocity());
    }

    buffer.clear();

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    {
        const juce::ScopedLock lock (voiceLock_);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float mix = 0.0f;
            for (auto& voice : voices_)
            {
                if (voice.position < voice.mono.size())
                {
                    mix += applyOutputCeiling (voice.mono[voice.position], activePreset_.outputCeilingDb);
                    ++voice.position;
                }
            }

            for (int ch = 0; ch < numChannels; ++ch)
                buffer.setSample (ch, sample, mix);
        }

        for (int i = voices_.size(); --i >= 0;)
        {
            if (voices_.getReference (i).position >= voices_.getReference (i).mono.size())
                voices_.remove (i);
        }
    }

    midi.clear();
}

juce::AudioProcessorEditor* TrapForgeAudioProcessor::createEditor()
{
    return new TrapForgeAudioProcessorEditor (*this);
}

void TrapForgeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream (destData, false);
    stream.writeInt (currentProgram_);
}

void TrapForgeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::MemoryInputStream stream (data, static_cast<std::size_t> (sizeInBytes), false);
    if (stream.getNumBytesRemaining() >= sizeof (juce::int32))
    {
        currentProgram_ = stream.readInt();
        refreshActivePreset();
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TrapForgeAudioProcessor();
}
