#include "HeadlessMidiRender.h"
#include "PluginProcessor.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include <cstdlib>
#include <iostream>
#include <memory>

namespace
{

void printUsage()
{
    std::cerr << "Usage: OfflineRender <output.wav> [program] [midiNote] [velocity] [seconds] [sampleRate]\n"
              << "  Instantiates JDUpgraded in-process (not the .vst3 bundle). See Vst3OfflineRender for bundle path.\n"
              << "  Defaults: program=0 note=60 velocity=100 seconds=2.0 sampleRate=44100\n";
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

    headless::MidiRenderArgs args;
    args.program = argc > 2 ? std::atoi (argv[2]) : 0;
    args.midiNote = argc > 3 ? std::atoi (argv[3]) : 60;
    args.velocity = argc > 4 ? std::atoi (argv[4]) : 100;
    args.seconds = argc > 5 ? std::atof (argv[5]) : 2.0;
    args.sampleRate = argc > 6 ? std::atof (argv[6]) : 44100.0;

    std::unique_ptr<juce::AudioProcessor> processor (createPluginFilter());
    if (processor == nullptr)
    {
        std::cerr << "Failed to create plugin processor\n";
        return 1;
    }

    juce::String error;
    if (! headless::renderProcessorToWav (*processor, outFile, args, error))
    {
        std::cerr << error << '\n';
        return 1;
    }

    return 0;
}
