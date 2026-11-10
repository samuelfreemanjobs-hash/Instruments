#pragma once
#include <JuceHeader.h>

/** Defaults tuned for Akai MPC Live (firmware 2.x / 3.x) standalone keygroups. */
struct KeygroupExportOptions
{
    /** MIDI note 36 = C1 (MPC pad/keyboard convention for this export). */
    int lowNote = 36;
    /** MIDI note 96 = C6 (inclusive chromatic map). */
    int highNote = 96;
    int semitoneStep = 1;
    int velocityLayers = 2;
    float bodySeconds = 1.15f;
    float releaseTailSeconds = 0.35f;
    bool use24Bit = false;
    float ampReleaseSeconds = 0.35f;
    float maxProgramMegabytes = 48.f;
    bool useExpansionsLayout = false;
    juce::String expansionsPackName { "DiskLordz_TrapLord" };
    juce::String folderPrefix { "DL_" };
    juce::String programName { "TrapKeygroup" };
};
