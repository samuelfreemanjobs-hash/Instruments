#include "DSP/PolyBlepOscillator.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

int main()
{
    pluggva::dsp::PolyBlepOscillator osc;
    osc.prepare (48000.0);
    osc.setFrequency (220.0f);

    float peak = 0.0f;
    float sum = 0.0f;
    constexpr int kSamples = 4800;
    for (int i = 0; i < kSamples; ++i)
    {
        const float s = osc.process (pluggva::Waveform::saw, 0.5f);
        peak = std::max (peak, std::abs (s));
        sum += s;
    }

    if (peak <= 0.05f || peak > 1.2f)
    {
        std::cerr << "saw peak out of range: " << peak << '\n';
        return EXIT_FAILURE;
    }

    const float mean = sum / static_cast<float> (kSamples);
    if (std::abs (mean) > 0.08f)
    {
        std::cerr << "saw DC offset too high: " << mean << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
