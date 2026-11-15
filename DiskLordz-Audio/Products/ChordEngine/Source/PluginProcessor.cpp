#include "PluginProcessor.h"
#include "PluginEditor.h"

ChordEngineAudioProcessor::ChordEngineAudioProcessor()
    : juce::AudioProcessor(BusesProperties()
                                 .withInput("Input", juce::AudioChannelSet::stereo(), true)
                                 .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

void ChordEngineAudioProcessor::prepareToPlay(double, int) {}

void ChordEngineAudioProcessor::releaseResources() {}

bool ChordEngineAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    return true;
}

void ChordEngineAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    juce::ignoreUnused(midi);
}

juce::AudioProcessorEditor* ChordEngineAudioProcessor::createEditor()
{
    return new ChordEngineAudioProcessorEditor(*this);
}

void ChordEngineAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::ignoreUnused(destData);
}

void ChordEngineAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    juce::ignoreUnused(data, sizeInBytes);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChordEngineAudioProcessor();
}
