#pragma once

namespace rev2trap
{
struct ParameterIDs
{
    static constexpr const char* outputGain = "outputGain";
    static constexpr const char* oscMix = "oscMix";
    static constexpr const char* osc2Detune = "osc2Detune";
    static constexpr const char* subLevel = "subLevel";
    static constexpr const char* cutoff = "cutoff";
    static constexpr const char* resonance = "resonance";
    static constexpr const char* filterEnv = "filterEnv";
    static constexpr const char* drive = "drive";
    static constexpr const char* ampAttack = "ampAttack";
    static constexpr const char* ampDecay = "ampDecay";
    static constexpr const char* ampSustain = "ampSustain";
    static constexpr const char* ampRelease = "ampRelease";
    static constexpr const char* filtAttack = "filtAttack";
    static constexpr const char* filtDecay = "filtDecay";
    static constexpr const char* filtSustain = "filtSustain";
    static constexpr const char* filtRelease = "filtRelease";
    static constexpr const char* wavetableBlend = "wavetableBlend";
    static constexpr const char* hardSync = "hardSync";
    static constexpr const char* filterFm = "filterFm";
    static constexpr const char* drift = "drift";
    static constexpr const char* trapMacro = "trapMacro";
};
} // namespace rev2trap
