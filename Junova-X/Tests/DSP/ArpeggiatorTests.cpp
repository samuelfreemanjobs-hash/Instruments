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
    arp.process (in, out, 0.0f, 2.0f, 120.0, 512);
    if (out.getNumEvents() < 2)
    {
        std::cerr << "bypass expected 2 note on events, got " << out.getNumEvents() << '\n';
        return EXIT_FAILURE;
    }

    arp.reset();
    in.clear();
    in.addEvent (juce::MidiMessage::noteOn (1, 48, static_cast<juce::uint8> (100)), 0);

    out.clear();
    int noteOns = 0;
    for (int b = 0; b < 8; ++b)
    {
        juce::MidiBuffer block;
        arp.process (in, block, 0.85f, 2.0f, 120.0, 512);
        for (const auto metadata : block)
            if (metadata.getMessage().isNoteOn())
                ++noteOns;
    }

    if (noteOns < 1)
    {
        std::cerr << "arp expected at least one note on, got " << noteOns << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "JunovaXTests OK\n";
    return EXIT_SUCCESS;
}
