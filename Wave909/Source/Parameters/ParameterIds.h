#pragma once

namespace wave909
{

constexpr int kStateVersion = 1;

struct ParameterIDs
{
    static constexpr const char* outputGain = "outputGain";
    static constexpr const char* wtPosition = "wtPosition";
    static constexpr const char* wtScan = "wtScan";
    static constexpr const char* filterCutoff = "filterCutoff";
    static constexpr const char* filterRes = "filterRes";
    static constexpr const char* filterCircuit = "filterCircuit";
    static constexpr const char* drive = "drive";
    static constexpr const char* crush = "crush";
    static constexpr const char* subLevel = "subLevel";
    static constexpr const char* detune = "detune";
    static constexpr const char* glide = "glide";
    static constexpr const char* tapeWobble = "tapeWobble";
    static constexpr const char* ampAttack = "ampAttack";
    static constexpr const char* ampDecay = "ampDecay";
    static constexpr const char* ampSustain = "ampSustain";
    static constexpr const char* ampRelease = "ampRelease";
    static constexpr const char* filtAttack = "filtAttack";
    static constexpr const char* filtDecay = "filtDecay";
    static constexpr const char* filtSustain = "filtSustain";
    static constexpr const char* filtRelease = "filtRelease";
    static constexpr const char* filtEnvAmt = "filtEnvAmt";
    static constexpr const char* stereoWidth = "stereoWidth";
};

} // namespace wave909
