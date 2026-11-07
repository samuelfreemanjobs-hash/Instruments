#pragma once

namespace autosampler
{
struct ParameterIDs
{
    static constexpr const char* outputGain = "outputGain";
    static constexpr const char* lowNote = "lowNote";
    static constexpr const char* highNote = "highNote";
    static constexpr const char* noteStep = "noteStep";
    static constexpr const char* noteMapPreset = "noteMapPreset";
    static constexpr const char* blackKeyPolicy = "blackKeyPolicy";
    static constexpr const char* recordSeconds = "recordSeconds";
    static constexpr const char* maxRecordSeconds = "maxRecordSeconds";
    static constexpr const char* gapMs = "gapMs";
    static constexpr const char* recordVelocity = "recordVelocity";
    static constexpr const char* multiVelocityLayers = "multiVelocityLayers";
    static constexpr const char* midiThru = "midiThru";
    static constexpr const char* recordUntilSilence = "recordUntilSilence";
    static constexpr const char* startAutoSample = "startAutoSample";
    static constexpr const char* exportPreset = "exportPreset";
    static constexpr const char* exportMpcKit = "exportMpcKit";
    static constexpr const char* reprocessPreset = "reprocessPreset";
    static constexpr const char* trimSilence = "trimSilence";
    static constexpr const char* normalizeCapture = "normalizeCapture";
    static constexpr const char* sustainLoop = "sustainLoop";
    static constexpr const char* trimThresholdDb = "trimThresholdDb";
    static constexpr const char* trimPadMs = "trimPadMs";
    static constexpr const char* normalizePeakDb = "normalizePeakDb";
};
} // namespace autosampler
