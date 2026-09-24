#include "PluginProcessor.h"

#include <JuceHeader.h>

#include <cmath>
#include <iostream>

namespace
{

float bufferPeak (const juce::AudioBuffer<float>& buf) noexcept
{
    float peak = 0.0f;
    for (int ch = 0; ch < buf.getNumChannels(); ++ch)
        peak = std::max (peak, buf.getMagnitude (ch, 0, buf.getNumSamples()));
    return peak;
}

} // namespace

int runProcessorSmokeTests()
{
    juce::ScopedJuceInitialiser_GUI juceInit;

    ProphetRev2TrapAudioProcessor processor;
    processor.prepareToPlay (48000.0, 512);

    juce::AudioBuffer<float> buffer (2, 512);
    juce::MidiBuffer midi;

    processor.setCurrentProgram (0);
    midi.addEvent (juce::MidiMessage::noteOn (1, 60, static_cast<juce::uint8> (100)), 0);

    float peakAfterNote = 0.0f;
    for (int block = 0; block < 8; ++block)
    {
        buffer.clear();
        midi.clear();
        if (block == 0)
            midi.addEvent (juce::MidiMessage::noteOn (1, 60, static_cast<juce::uint8> (100)), 0);
        processor.processBlock (buffer, midi);
        peakAfterNote = std::max (peakAfterNote, bufferPeak (buffer));
    }

    if (peakAfterNote < 1.0e-4f)
    {
        std::cerr << "Processor smoke: expected non-silent output after note-on (peak=" << peakAfterNote << ")\n";
        return 1;
    }

    juce::MemoryBlock state;
    processor.getStateInformation (state);
    if (state.getSize() < 8)
    {
        std::cerr << "Processor smoke: getStateInformation returned empty block\n";
        return 1;
    }

    processor.setCurrentProgram (5);
    processor.setStateInformation (state.getData(), static_cast<int> (state.getSize()));
    if (processor.getCurrentProgram() != 0)
    {
        std::cerr << "Processor smoke: state recall did not restore program index\n";
        return 1;
    }

    return 0;
}
