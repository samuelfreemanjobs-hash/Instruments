#include "PluginProcessor.h"

int main()
{
    Rev2TrapAudioProcessor proc;
    proc.prepareToPlay (44100.0, 512);
    proc.setCurrentProgram (0);
    juce::AudioBuffer<float> buf (2, 512);
    juce::MidiBuffer midi;
    midi.addEvent (juce::MidiMessage::noteOn (1, 60, static_cast<juce::uint8> (100)), 0);
    proc.processBlock (buf, midi);
    return 0;
}
