#pragma once

namespace disklordz::rompler
{

struct ParameterIDs
{
    static constexpr const char* outputGain = "outputGain";
    static constexpr const char* tone1 = "tone1";
    static constexpr const char* tone2 = "tone2";
    static constexpr const char* tone3 = "tone3";
    static constexpr const char* tone4 = "tone4";
    static constexpr const char* macroCharacter = "macroCharacter";
    static constexpr const char* macroBrightness = "macroBrightness";
    static constexpr const char* macroDrive = "macroDrive";
    static constexpr const char* macroSpace = "macroSpace";
    static constexpr const char* macroMovement = "macroMovement";
    static constexpr const char* macroWidth = "macroWidth";
    static constexpr const char* macroDecay = "macroDecay";
    static constexpr const char* macroCrush = "macroCrush";
    static constexpr const char* filterCutoff = "filterCutoff";
    static constexpr const char* ampAttack = "ampAttack";
    static constexpr const char* ampDecay = "ampDecay";
    static constexpr const char* ampSustain = "ampSustain";
    static constexpr const char* ampRelease = "ampRelease";
};

} // namespace disklordz::rompler
