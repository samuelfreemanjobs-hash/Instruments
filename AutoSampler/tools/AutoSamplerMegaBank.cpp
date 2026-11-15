#include "../Source/Engine/MegaBankRunner.h"

#include <juce_core/juce_core.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include <iostream>

namespace
{

void printUsage()
{
    std::cerr << "Usage:\n"
              << "  AutoSamplerMegaBank <mega_bank.json> <outputDirectory>\n"
              << "  AutoSamplerMegaBank --template <output.json>\n"
              << "Up to " << autosampler::kMaxSourcePlugins << " VST3 sources; resume via mega_bank_progress.json\n";
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

    juce::String error;
    if (juce::String (argv[1]) == "--template")
    {
        if (argc < 3)
        {
            printUsage();
            return 1;
        }

        if (! autosampler::MegaBankRunner::writeMegaBankTemplate (juce::File (argv[2]), error))
        {
            std::cerr << error << '\n';
            return 1;
        }

        std::cout << "Wrote overnight mega bank template: " << argv[2] << '\n';
        return 0;
    }

    if (argc < 3)
    {
        printUsage();
        return 1;
    }

    autosampler::MegaBankJob job;
    if (! autosampler::MegaBankRunner::loadJob (juce::File (argv[1]), job, error))
    {
        std::cerr << error << '\n';
        return 1;
    }

    autosampler::MegaBankRunner::RunSummary summary;
    const bool ok = autosampler::MegaBankRunner::runJob (job, juce::File (argv[2]), error, &summary);

    if (summary.skipped > 0)
    {
        std::cerr << "Mega bank finished with skipped presets (" << summary.skipped << "/" << summary.sourcesTotal << "):\n";
        for (int i = 0; i < summary.skippedPresets.size(); ++i)
            std::cerr << "  - " << summary.skippedPresets[i] << ": " << summary.skippedReasons[i] << '\n';
        std::cerr << error << '\n';
        return 1;
    }

    if (! ok)
    {
        std::cerr << error << '\n';
        return 1;
    }

    std::cout << "Mega bank '" << job.name << "' exported " << summary.completed << " / " << summary.sourcesTotal
              << " source VST3(s).\n";
    return 0;
}
