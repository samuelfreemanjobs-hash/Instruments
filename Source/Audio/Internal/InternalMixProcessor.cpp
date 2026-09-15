#include "InternalMixProcessor.h"

namespace vmpc::audio::internal
{
InternalMixProcessor::InternalMixProcessor(const juce::String& pluginName)
    : AudioProcessor(stereoLayout())
    , name(pluginName)
{
}

juce::AudioProcessor::BusesProperties InternalMixProcessor::stereoLayout()
{
    return BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true);
}

bool InternalMixProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void InternalMixProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    juce::ValueTree state("VMPCInternalState");
    for (auto* param : getParameters())
    {
        if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
            state.setProperty(floatParam->paramID, floatParam->get(), nullptr);
    }

    if (auto xml = state.createXml())
        copyXmlToBinary(*xml, dest);
}

void InternalMixProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
    {
        const auto state = juce::ValueTree::fromXml(*xml);
        for (auto* param : getParameters())
        {
            if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param))
            {
                if (state.hasProperty(floatParam->paramID))
                {
                    const float raw = static_cast<float>(state.getProperty(floatParam->paramID));
                    floatParam->setValueNotifyingHost(floatParam->getNormalisableRange().convertTo0to1(raw));
                }
            }
        }
    }
}
} // namespace vmpc::audio::internal
