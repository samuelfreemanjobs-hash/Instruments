#pragma once

#include "JdEnvelopeScale.h"
#include "JdPatchLayout.h"

#include <cstdint>

namespace jdupgraded::preset
{

struct JdTonePitchMod final
{
    float pitchLevel0Mult = 1.0f;
    float pitchLevel1Mult = 1.0f;
    float pitchLevel2Mult = 1.0f;
    float pitchAttackSec = 0.002f;
    float pitchDecaySec = 0.15f;
    float pitchReleaseSec = 0.25f;
    float lfo1RateHz = 0.0f;
    float lfo1PitchDepthSemis = 0.0f;
};

inline JdTonePitchMod decodeTonePitchMod (const std::uint8_t* patch,
                                          std::size_t patchBytes,
                                          std::size_t toneIndex) noexcept
{
    JdTonePitchMod out{};
    if (patch == nullptr || patchBytes < kJdPatchTotalBytes || toneIndex >= kJdTonesPerPatch)
        return out;

    const auto base = kJdToneBlockOffset (toneIndex);
    out.pitchLevel0Mult = jdPitchLevelToMultiplier (patch[base + kJdTonePitchEnvLevel0]);
    out.pitchLevel1Mult = jdPitchLevelToMultiplier (patch[base + kJdTonePitchEnvLevel1]);
    out.pitchLevel2Mult = jdPitchLevelToMultiplier (patch[base + kJdTonePitchEnvLevel2]);
    out.pitchAttackSec = jdEnvelopeTimeToSeconds (patch[base + kJdTonePitchEnvTime1]);
    out.pitchDecaySec = jdEnvelopeTimeToSeconds (patch[base + kJdTonePitchEnvTime2]);
    out.pitchReleaseSec = jdEnvelopeTimeToSeconds (patch[base + kJdTonePitchEnvTime3]);

    const auto lfoWave = patch[base + kJdToneLfo1Waveform] & 0x7F;
    if (lfoWave <= 4)
    {
        out.lfo1RateHz = jdLfoRateToHz (patch[base + kJdToneLfo1Rate]);
        out.lfo1PitchDepthSemis = jdLfoSensToPitchDepthSemis (patch[base + kJdToneLfo1Sens]);
    }

    return out;
}

} // namespace jdupgraded::preset
