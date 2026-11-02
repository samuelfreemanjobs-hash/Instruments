#pragma once

namespace sp1200
{
/** Drives LCD line-2 scrub target (spec §4.2 editField). */
enum class LcdEditField
{
    none,
    busCutoff,
    busResonance,
    bpm,
    swing,
    padTune,
    padLevel,
    padDecay
};

inline const char* lcdEditFieldLabel (LcdEditField field) noexcept
{
    switch (field)
    {
        case LcdEditField::busCutoff: return "SSM CUTOFF";
        case LcdEditField::busResonance: return "SSM RES";
        case LcdEditField::bpm: return "BPM";
        case LcdEditField::swing: return "SWING";
        case LcdEditField::padTune: return "PAD TUNE";
        case LcdEditField::padLevel: return "PAD LEVEL";
        case LcdEditField::padDecay: return "PAD DECAY";
        default: return "";
    }
}

} // namespace sp1200
