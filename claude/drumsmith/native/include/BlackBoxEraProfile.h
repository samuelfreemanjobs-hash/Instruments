#pragma once

#include <cstdint>

/** Black Box era profile — mirrors kits/black-box-collection/profiles/*.json
 *  Shared JUCE/C++ drum engine target (planned). Sample kits use the Python
 *  generator with the same knobs until this engine is wired.
 */
struct BlackBoxEraProfile
{
    const char* sku = "BB96";
    const char* title = "BLACK BOX 96";
    const char* identity = "The Original Machine";

    int bitDepth = 12;
    float lofiSr = 26040.0f;
    float tapeLpfHz = 11800.0f;
    float kickPitchSt = 48.0f;
    float kickClipDb = 4.5f;
    float snareClipDb = 5.5f;
    float hatCenterHz = 9200.0f;
    float kickDuration = 0.35f;
    float ampDecayKick = 0.14f;
    float cowF1 = 540.0f;
    float cowF2 = 800.0f;
    float rootBiasSt = 0.0f;

    enum class Character : std::uint8_t
    {
        HollowTape = 0,   // BB93
        Signature,        // BB96
        TrunkBass,        // BB98
        DigitalSouth      // BB2000
    };

    Character character = Character::Signature;
};

enum class DirtyCircuitsMode : std::uint8_t
{
    Clean = 0,
    Dirty,
    Ruined
};
