#pragma once

#include <JuceHeader.h>
#include <map>

namespace vmpc::audio::internal
{
/** Apply raw engineering values (dB, ms, etc.) by AudioParameter ID. */
inline void applyRawParameterMap(juce::AudioProcessor& processor,
                                 const std::map<juce::String, float>& rawByParamId)
{
    for (auto* param : processor.getParameters())
    {
        auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param);
        if (floatParam == nullptr)
            continue;

        const auto it = rawByParamId.find(floatParam->paramID);
        if (it == rawByParamId.end())
            continue;

        const float normalised = floatParam->getNormalisableRange().convertTo0to1(it->second);
        floatParam->setValueNotifyingHost(normalised);
    }
}
} // namespace vmpc::audio::internal
