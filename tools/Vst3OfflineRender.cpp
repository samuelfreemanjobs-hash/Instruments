#include "HeadlessMidiRender.h"

#include <cstdlib>
#include <iostream>

namespace
{

void printUsage()
{
    std::cerr << "Usage: Vst3OfflineRender <plugin.vst3> <output.wav> [program] [midiNote] [velocity] [seconds] [sampleRate]\n"
              << "  Loads the VST3 bundle through JUCE's VST3 host (same MIDI→WAV path as OfflineRender).\n"
              << "  Defaults: program=0 note=60 velocity=100 seconds=2.0 sampleRate=44100\n";
}

std::unique_ptr<juce::AudioPluginInstance> loadVst3 (const juce::File& bundle, double sampleRate, int blockSize, juce::String& error)
{
    if (! bundle.exists())
    {
        error = "VST3 bundle not found: " + bundle.getFullPathName();
        return nullptr;
    }

    juce::VST3PluginFormat format;
    juce::OwnedArray<juce::PluginDescription> types;
    format.findAllTypesForFile (types, bundle.getFullPathName());

    if (types.isEmpty())
    {
        error = "No plugin types found in " + bundle.getFullPathName();
        return nullptr;
    }

    return format.createInstanceFromDescription (*types.getFirst(), sampleRate, blockSize, error);
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

    const juce::File bundle (argv[1]);
    const juce::File outFile (argv[2]);

    headless::MidiRenderArgs args;
    args.program = argc > 3 ? std::atoi (argv[3]) : 0;
    args.midiNote = argc > 4 ? std::atoi (argv[4]) : 60;
    args.velocity = argc > 5 ? std::atoi (argv[5]) : 100;
    args.seconds = argc > 6 ? std::atof (argv[6]) : 2.0;
    args.sampleRate = argc > 7 ? std::atof (argv[7]) : 44100.0;

    juce::String error;
    auto instance = loadVst3 (bundle, args.sampleRate, args.blockSize, error);
    if (instance == nullptr)
    {
        std::cerr << error << '\n';
        return 1;
    }

    if (! headless::renderProcessorToWav (*instance, outFile, args, error))
    {
        std::cerr << error << '\n';
        return 1;
    }

    return 0;
}
