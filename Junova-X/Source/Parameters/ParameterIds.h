#pragma once

namespace junovax
{
struct ParameterIDs
{
    static constexpr const char* masterGain = "masterGain";

    static constexpr const char* ampAttack = "ampAttack";
    static constexpr const char* ampDecay = "ampDecay";
    static constexpr const char* ampSustain = "ampSustain";
    static constexpr const char* ampRelease = "ampRelease";

    static constexpr const char* filtAttack = "filtAttack";
    static constexpr const char* filtDecay = "filtDecay";
    static constexpr const char* filtSustain = "filtSustain";
    static constexpr const char* filtRelease = "filtRelease";

    static constexpr const char* filterCutoff = "filterCutoff";
    static constexpr const char* filterRes = "filterRes";
    static constexpr const char* hpfEnabled = "hpfEnabled";

    static constexpr const char* chorusMode = "chorusMode";

    static constexpr const char* diagTestTone = "diagTestTone";
    static constexpr const char* diagToneFreq = "diagToneFreq";
};
} // namespace junovax
