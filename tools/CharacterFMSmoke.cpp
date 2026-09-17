#include "DSP/AlgorithmData.h"
#include "DSP/FMEngine.h"
#include "DSP/ModulationGraph.h"

#include <cmath>
#include <cstdio>
#include <vector>

int main()
{
    int failed = 0;

    for (int i = 0; i < 32; ++i)
    {
        const auto& g = characterfm::dsp::getAlgorithmGraph (i);
        if (! g.validate())
        {
            std::fprintf (stderr, "Algorithm %d failed validate\n", i);
            ++failed;
        }
    }

    characterfm::dsp::FMEngine engine;
    engine.reset (44100.0);

    characterfm::dsp::VoiceParams params;
    params.algorithmIndex = 0;
    params.operators[0].ratio = 1.0f;
    params.operators[0].level = 0.9f;
    params.operators[1].ratio = 2.0f;
    params.operators[1].level = 0.5f;
    engine.setGlobalVoiceParams (params);
    engine.noteOn (60, 0.9f);

    std::vector<float> mono (2205, 0.0f);
    engine.renderBlock (mono.data(), mono.data(), static_cast<int> (mono.size()), 0.75f);

    double sumSq = 0.0;
    for (float s : mono)
        sumSq += static_cast<double> (s) * static_cast<double> (s);

    const double rms = std::sqrt (sumSq / static_cast<double> (mono.size()));
    if (rms < 0.001)
    {
        std::fprintf (stderr, "Expected non-silent render, rms=%f\n", rms);
        ++failed;
    }

    engine.noteOff (60);
    std::fill (mono.begin(), mono.end(), 0.0f);
    for (int i = 0; i < 44100; ++i)
        engine.renderBlock (mono.data(), mono.data(), 1, 0.75f);

    if (failed == 0)
        std::printf ("CharacterFMSmoke OK (rms=%f)\n", rms);

    return failed == 0 ? 0 : 1;
}
