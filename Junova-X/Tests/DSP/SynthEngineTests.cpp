#include "DSP/SynthEngine.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

int main()
{
    junovax::dsp::SynthEngine engine;
    engine.prepare (48000.0, 512);
    junovax::dsp::RuntimeParams p;
    p.filterCutoff = 0.55f;
    p.filterRes = 0.2f;
    p.dcoSawOn = true;
    p.dcoPulseOn = true;
    engine.setParams (p);

    juce::AudioBuffer<float> buffer (2, 4096);
    buffer.clear();
    juce::MidiBuffer midi;
    midi.addEvent (juce::MidiMessage::noteOn (1, 60, static_cast<juce::uint8> (110)), 0);

    engine.render (buffer, midi);

    double sum = 0.0;
    double sumSq = 0.0;
    const auto* data = buffer.getReadPointer (0);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        sum += data[i];
        sumSq += static_cast<double> (data[i]) * static_cast<double> (data[i]);
    }
    const double mean = sum / static_cast<double> (buffer.getNumSamples());
    if (std::abs (mean) > 0.02)
    {
        std::cerr << "DC offset too high: " << mean << '\n';
        return EXIT_FAILURE;
    }

    const float peak = engine.readAndResetPeak();
    if (peak > juce::Decibels::decibelsToGain (-0.5f) + 0.01f)
    {
        std::cerr << "Peak exceeds -0.5 dBFS: " << peak << '\n';
        return EXIT_FAILURE;
    }

    if (peak < 0.001f)
    {
        std::cerr << "Silent render\n";
        return EXIT_FAILURE;
    }

    std::cout << "SynthEngineTests OK\n";
    return EXIT_SUCCESS;
}