#include "DSP/WavetableBank.h"
#include "DSP/Filter/ZdfLowpass.h"
#include "Engine/VoiceManager.h"
#include "Engine/SynthParams.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

int main()
{
    wave909::dsp::WavetableBank bank;
    bank.rebuild();

    const auto& t0 = bank.getTable (0);
    const auto& t63 = bank.getTable (63);

    float peak0 = 0.0f;
    float peak63 = 0.0f;
    for (float s : t0)
        peak0 = std::max (peak0, std::abs (s));
    for (float s : t63)
        peak63 = std::max (peak63, std::abs (s));

    if (peak0 <= 0.0f || peak0 > 1.0f)
    {
        std::cerr << "table 0 peak out of range: " << peak0 << '\n';
        return EXIT_FAILURE;
    }

    if (peak63 <= 0.0f || peak63 > 1.0f)
    {
        std::cerr << "table 63 peak out of range: " << peak63 << '\n';
        return EXIT_FAILURE;
    }

    const float morph = bank.sampleMorph (0.25, 0.5f);
    if (! std::isfinite (morph))
    {
        std::cerr << "non-finite morph sample\n";
        return EXIT_FAILURE;
    }

    wave909::engine::VoiceManager voices;
    voices.prepare (48000.0);
    wave909::SynthParams p;
    p.wtPosition = 0.5f;
    p.filterCutoff = 0.6f;
    p.ampSustain = 0.9f;
    voices.setParams (p);
    voices.noteOn (36, 1.0f);

    std::vector<float> left (4096, 0.0f);
    std::vector<float> right (4096, 0.0f);
    voices.render (left.data(), right.data(), static_cast<int> (left.size()));

    float renderPeak = 0.0f;
    for (float s : left)
        renderPeak = std::max (renderPeak, std::abs (s));
    if (renderPeak < 1.0e-4f)
    {
        std::cerr << "engine render peak too low: " << renderPeak << '\n';
        return EXIT_FAILURE;
    }

    wave909::dsp::ZdfLowpass filter;
    filter.prepare (48000.0);
    float lpPeak = 0.0f;
    for (int i = 0; i < 500; ++i)
    {
        const float x = (i == 0) ? 1.0f : 0.0f;
        const float y = filter.process (x, 0.5f, 0.2f);
        lpPeak = std::max (lpPeak, std::abs (y));
    }
    if (! std::isfinite (lpPeak) || lpPeak <= 0.0f)
    {
        std::cerr << "filter unstable: " << lpPeak << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Wave909Tests OK peak0=" << peak0 << " peak63=" << peak63
              << " renderPeak=" << renderPeak << " lpPeak=" << lpPeak << '\n';
    return EXIT_SUCCESS;
}
