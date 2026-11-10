#pragma once
#include <JuceHeader.h>

struct KeygroupExportOptions
{
    int lowNote = 24;
    int highNote = 84;
    int semitoneStep = 3;
    int velocityLayers = 2;
    float bodySeconds = 2.f;
    float releaseTailSeconds = 0.5f;
    bool use24Bit = true;
    juce::String programName { "TrapLordKeygroup" };
};
