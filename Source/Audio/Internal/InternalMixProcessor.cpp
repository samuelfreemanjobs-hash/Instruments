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
} // namespace vmpc::audio::internal
