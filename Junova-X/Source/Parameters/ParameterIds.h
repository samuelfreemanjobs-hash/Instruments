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
    static constexpr const char* hpfMode = "hpfMode";

    static constexpr const char* chorusMode = "chorusMode";
    static constexpr const char* chorusHiss = "chorusHiss";
    static constexpr const char* voiceMode = "voiceMode";

    static constexpr const char* lfoWaveform = "lfoWaveform";
    static constexpr const char* lfoRate = "lfoRate";
    static constexpr const char* lfoDelay = "lfoDelay";
    static constexpr const char* lfoBpmSync = "lfoBpmSync";
    static constexpr const char* glide = "glide";

    static constexpr const char* dcoOctave = "dcoOctave";
    static constexpr const char* dcoPulseOn = "dcoPulseOn";
    static constexpr const char* dcoSawOn = "dcoSawOn";
    static constexpr const char* dcoSubOn = "dcoSubOn";
    static constexpr const char* dcoLfoMod = "dcoLfoMod";
    static constexpr const char* dcoPwm = "dcoPwm";
    static constexpr const char* pwmSourceLfo = "pwmSourceLfo";
    static constexpr const char* dcoSubLvl = "dcoSubLvl";
    static constexpr const char* dcoSubOct = "dcoSubOct";
    static constexpr const char* dcoNoise = "dcoNoise";

    static constexpr const char* vcfEnv = "vcfEnv";
    static constexpr const char* vcfLfo = "vcfLfo";
    static constexpr const char* vcfKey = "vcfKey";
    static constexpr const char* vcfEnvInv = "vcfEnvInv";
    static constexpr const char* vcfDrive = "vcfDrive";

    static constexpr const char* vcaGateMode = "vcaGateMode";
    static constexpr const char* envVintageCurve = "envVintageCurve";

    static constexpr const char* drift = "drift";
    static constexpr const char* detune = "detune";
    static constexpr const char* width = "width";

    static constexpr const char* arpEnabled = "arpEnabled";
    static constexpr const char* arpDirection = "arpDirection";
    static constexpr const char* arpRange = "arpRange";
    static constexpr const char* arpRate = "arpRate";
    static constexpr const char* arpLatch = "arpLatch";

    static constexpr const char* editorZoom = "editorZoom";
    static constexpr const char* oversampleMode = "oversampleMode";

    static constexpr const char* diagTestTone = "diagTestTone";
    static constexpr const char* diagToneFreq = "diagToneFreq";

    // Legacy (kept for preset migration)
    static constexpr const char* hpfEnabled = "hpfEnabled";
    static constexpr const char* hpfCutoff = "hpfCutoff";
};
} // namespace junovax