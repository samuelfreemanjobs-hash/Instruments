#include "ParameterLayout.h"

#include "ParameterIds.h"

namespace characterfm
{

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> (params::masterGain,
                                                             "Master",
                                                             juce::NormalisableRange<float> (0.0f, 1.0f),
                                                             0.75f));

    layout.add (std::make_unique<juce::AudioParameterInt> (params::algorithm,
                                                           "Algorithm",
                                                           0,
                                                           31,
                                                           0));

    for (int op = 1; op <= 6; ++op)
    {
        const juce::String ratioId (params::opRatio (op));
        const juce::String levelId (params::opLevel (op));
        layout.add (std::make_unique<juce::AudioParameterFloat> (ratioId,
                                                                 "OP" + juce::String (op) + " Ratio",
                                                                 juce::NormalisableRange<float> (0.5f, 32.0f, 0.01f),
                                                                 op == 1 ? 1.0f : static_cast<float> (op)));
        layout.add (std::make_unique<juce::AudioParameterFloat> (levelId,
                                                                 "OP" + juce::String (op) + " Level",
                                                                 juce::NormalisableRange<float> (0.0f, 1.0f),
                                                                 op == 1 ? 0.85f : 0.4f));
    }

    return layout;
}

} // namespace characterfm
