#pragma once

namespace autosampler
{
struct ParameterIDs
{
    static constexpr const char* outputGain = "outputGain";
    static constexpr const char* lowNote = "lowNote";
    static constexpr const char* highNote = "highNote";
    static constexpr const char* noteStep = "noteStep";
    static constexpr const char* recordSeconds = "recordSeconds";
    static constexpr const char* gapMs = "gapMs";
    static constexpr const char* recordVelocity = "recordVelocity";
    static constexpr const char* midiThru = "midiThru";
    static constexpr const char* startAutoSample = "startAutoSample";
    static constexpr const char* exportPreset = "exportPreset";
    static constexpr const char* recordProgress = "recordProgress";
    static constexpr const char* autoSampleState = "autoSampleState";
};
} // namespace autosampler
