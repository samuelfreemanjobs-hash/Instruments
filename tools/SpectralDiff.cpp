#include "WavCompare.h"

#include <juce_gui_extra/juce_gui_extra.h>

#include <cstdlib>
#include <iostream>

namespace
{

void printUsage()
{
    std::cerr << "Usage: SpectralDiff <reference.wav> <test.wav> [--max-rms-db N] [--max-spectral-db N]\n"
              << "  Fail when RMS diff (dB) is above max-rms-db (default -60) or mean spectral bin\n"
              << "  error (dB) is above max-spectral-db (default 1.0).\n";
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

    const juce::File fileA (argv[1]);
    const juce::File fileB (argv[2]);
    float maxRmsDb = -60.0f;
    float maxSpectralDb = 1.0f;

    for (int i = 3; i < argc; ++i)
    {
        const juce::String arg (argv[i]);
        if (arg == "--max-rms-db" && i + 1 < argc)
            maxRmsDb = static_cast<float> (std::atof (argv[++i]));
        else if (arg == "--max-spectral-db" && i + 1 < argc)
            maxSpectralDb = static_cast<float> (std::atof (argv[++i]));
    }

    const auto result = jdupgraded::tools::compareWavFiles (fileA, fileB);
    if (! result.ok)
    {
        std::cerr << "Compare failed: " << result.error << '\n';
        return 1;
    }

    std::cout << "samples=" << result.numSamples << " sr=" << result.sampleRate << '\n'
              << "rms_diff_db=" << result.rmsDiffDb << '\n'
              << "spectral_mean_diff_db=" << result.spectralMeanDiffDb << '\n';

    bool pass = true;
    if (result.rmsDiffDb > maxRmsDb)
    {
        std::cerr << "FAIL: RMS diff " << result.rmsDiffDb << " dB > limit " << maxRmsDb << " dB\n";
        pass = false;
    }

    if (result.spectralMeanDiffDb > maxSpectralDb)
    {
        std::cerr << "FAIL: spectral diff " << result.spectralMeanDiffDb << " dB > limit " << maxSpectralDb
                  << " dB\n";
        pass = false;
    }

    return pass ? 0 : 2;
}
