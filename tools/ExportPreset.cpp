#include "PluginProcessor.h"

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include <cstdlib>
#include <iostream>

namespace
{

void printUsage()
{
    std::cerr << "Usage: ExportPreset <output.jdpreset> [programIndex]\n";
}

} // namespace

int main (int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    if (argc < 2)
    {
        printUsage();
        return 1;
    }

    const juce::File outFile (argv[1]);
    const int program = argc > 2 ? std::atoi (argv[2]) : 0;

    std::unique_ptr<juce::AudioProcessor> processor (createPluginFilter());
    if (processor == nullptr)
    {
        std::cerr << "Failed to create processor\n";
        return 1;
    }

    auto* jd = dynamic_cast<JDUpgradedAudioProcessor*> (processor.get());
    if (jd == nullptr)
    {
        std::cerr << "Unexpected processor type\n";
        return 1;
    }

    jd->prepareToPlay (44100.0, 512);
    jd->setCurrentProgram (program);

    if (! jd->exportApvtsPresetToFile (outFile))
    {
        std::cerr << "Export failed\n";
        return 1;
    }

    std::cout << "Wrote preset program " << program << " to " << outFile.getFullPathName() << '\n';
    return 0;
}
