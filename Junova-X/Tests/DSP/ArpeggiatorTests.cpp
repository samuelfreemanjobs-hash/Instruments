#include "DSP/Arpeggiator.h"

#include <cstdlib>
#include <iostream>

int main()
{
    junovax::dsp::Arpeggiator arp;
    arp.prepare (48000.0);

    juce::MidiBuffer in;
    in.addEvent (juce::MidiMessage::noteOn (1, 60, static_cast<juce::uint8> (100)), 0);
    in.addEvent (juce::MidiMessage::noteOn (1, 64, static_cast<juce::uint8> (100)), 0);

    juce::MidiBuffer out;
    junovax::dsp::ArpHostContext host;
    host.bpm = 120.0;
    host.ppqPosition = 0.0;
    host.ppqValid = true;

    arp.process (in, out, 0.0f, 2.0f, false, host, 512);
    if (out.getNumEvents() < 2)
    {
        std::cerr << "bypass expected 2 note on events, got " << out.getNumEvents() << '\n';
        return EXIT_FAILURE;
    }

    arp.reset();
    in.clear();
    in.addEvent (juce::MidiMessage::noteOn (1, 48, static_cast<juce::uint8> (100)), 0);

    int noteOns = 0;
    host.ppqPosition = 0.0;
    for (int b = 0; b < 16; ++b)
    {
        juce::MidiBuffer block;
        host.ppqPosition = static_cast<double> (b) * 512.0 / 48000.0 * (120.0 / 60.0);
        arp.process (in, block, 0.85f, 2.0f, false, host, 512);
        for (const auto metadata : block)
            if (metadata.getMessage().isNoteOn())
                ++noteOns;
    }

    if (noteOns < 1)
    {
        std::cerr << "PPQ arp expected at least one note on, got " << noteOns << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "JunovaXTests OK\n";
    return EXIT_SUCCESS;
}
