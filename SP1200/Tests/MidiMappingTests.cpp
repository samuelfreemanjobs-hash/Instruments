#include "Engine/MidiMapping.h"
#include "Engine/SamplerEngine.h"

#include <iostream>

int main()
{
    sp1200::MidiMapping sq;
    sq.resetToDefaults();
    if (sq.padNote (0) != 36 || sq.faderCc (0) != 20)
    {
        std::cerr << "SQ-1 default map failed\n";
        return 1;
    }

    sp1200::MidiMapping renaissance;
    renaissance.applyMpcRenaissanceProfile();
    if (renaissance.padNote (15) != 51 || renaissance.faderCc (15) != 16)
    {
        std::cerr << "Renaissance profile map failed\n";
        return 1;
    }

    sp1200::SamplerEngine engine;
    engine.applyMpcRenaissanceController();
    if (engine.midiChannel() != 1 || ! engine.midiOmni())
    {
        std::cerr << "Renaissance controller setup failed\n";
        return 1;
    }

    std::cout << "SP1200MidiMappingTests OK\n";
    return 0;
}
