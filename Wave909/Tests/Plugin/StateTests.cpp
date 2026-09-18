#include "PluginProcessor.h"

#include <juce_audio_processors/juce_audio_processors.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>

int main()
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    auto processor = std::make_unique<Wave909AudioProcessor>();
    processor->prepareToPlay (48000.0, 512);
    processor->setCurrentProgram (2);

    if (auto* cutoff = processor->getApvts().getParameter (wave909::ParameterIDs::filterCutoff))
        cutoff->setValueNotifyingHost (cutoff->convertTo0to1 (0.42f));

    juce::MemoryBlock saved;
    processor->getStateInformation (saved);

    if (auto* cutoff = processor->getApvts().getParameter (wave909::ParameterIDs::filterCutoff))
        cutoff->setValueNotifyingHost (cutoff->convertTo0to1 (0.91f));

    processor->setCurrentProgram (0);
    processor->setStateInformation (saved.getData(), static_cast<int> (saved.getSize()));

    if (processor->getCurrentProgram() != 2)
    {
        std::cerr << "state restore failed: program=" << processor->getCurrentProgram() << '\n';
        return EXIT_FAILURE;
    }

    const float restored = processor->getApvts().getRawParameterValue (wave909::ParameterIDs::filterCutoff)->load();
    if (std::abs (restored - 0.42f) > 0.05f)
    {
        std::cerr << "state restore failed: cutoff=" << restored << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Wave909StateTests OK\n";
    return EXIT_SUCCESS;
}
