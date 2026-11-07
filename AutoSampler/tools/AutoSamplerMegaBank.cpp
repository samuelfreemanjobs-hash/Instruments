#include "../Source/Engine/MegaBankRunner.h"

#include <juce_core/juce_core.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include <iostream>

namespace
{

void printUsage()
{
    std::cerr << "Usage: AutoSamplerMegaBank <mega_bank.json> <outputDirectory>\n"
              << "  Up to " << autosampler::kMaxSourcePlugins << " source VST3 paths; see examples/mega_bank.example.json\n";
}

} // namespace

int main (int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    if (argc < 3)
    {
        printUsage();
        return 1;
    }

    autosampler::MegaBankJob job;
    juce::String error;
    if (! autosampler::MegaBankRunner::loadJob (juce::File (argv[1]), job, error))
    {
        std::cerr << error << '\n';
        return 1;
    }

    if (! autosampler::MegaBankRunner::runJob (job, juce::File (argv[2]), error))
    {
        std::cerr << error << '\n';
        return 1;
    }

    std::cout << "Mega bank '" << job.name << "' exported with " << job.sources.size() << " source VST3(s).\n";
    return 0;
}
