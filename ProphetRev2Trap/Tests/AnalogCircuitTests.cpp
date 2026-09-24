#include "DSP/AnalogCircuit.h"

#include <cmath>
#include <iostream>

int main()
{
    const float clipped = prophetrev2::dsp::softClip (2.0f, 0.5f);
    if (std::abs (clipped) > 1.0f)
    {
        std::cerr << "softClip should bound output\n";
        return 1;
    }

    const float hz = prophetrev2::dsp::keyTrackedCutoffHz (1000.0f, 72, 0.5f);
    if (hz <= 1000.0f)
    {
        std::cerr << "key track should raise cutoff for note above middle C\n";
        return 1;
    }

    return 0;
}
