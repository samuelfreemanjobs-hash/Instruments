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
    static constexpr const char* hpfCutoff = "hpfCutoff";

    static constexpr const char* chorusMode = "chorusMode";
    static constexpr const char* voiceMode = "voiceMode";

    static constexpr const char* lfoRate = "lfoRate";
    static constexpr const char* lfoDelay = "lfoDelay";
    static constexpr const char* glide = "glide";

    static constexpr const char* dcoLfoMod = "dcoLfoMod";
    static constexpr const char* dcoPwm = "dcoPwm";
    static constexpr const char* dcoSubLvl = "dcoSubLvl";
    static constexpr const char* dcoNoise = "dcoNoise";

    static constexpr const char* vcfEnv = "vcfEnv";
    static constexpr const char* vcfLfo = "vcfLfo";
    static constexpr const char* vcfKey = "vcfKey";

    static constexpr const char* drift = "drift";
    static constexpr const char* detune = "detune";
    static constexpr const char* width = "width";

    static constexpr const char* arpRange = "arpRange";
    static constexpr const char* arpRate = "arpRate";

    static constexpr const char* diagTestTone = "diagTestTone";
    static constexpr const char* diagToneFreq = "diagToneFreq";
};
} // namespace junovax
