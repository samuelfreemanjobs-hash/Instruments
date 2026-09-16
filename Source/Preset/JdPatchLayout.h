#pragma once

#include <cstddef>
#include <cstdint>

namespace jdupgraded::preset
{

/** JD-800 / JD-990 patch RAM layout (documented parameter offsets, clean-room decode only). */
inline constexpr std::size_t kJdPatchCommonBytes = 50;
inline constexpr std::size_t kJdPatchEffectBytes = 46;
inline constexpr std::size_t kJdPatchToneBytes = 72;
inline constexpr std::size_t kJdTonesPerPatch = 4;
inline constexpr std::size_t kJdPatchTotalBytes =
    kJdPatchCommonBytes + kJdPatchEffectBytes + kJdPatchToneBytes * kJdTonesPerPatch;

inline constexpr std::uint8_t kRolandManufacturer = 0x41;
inline constexpr std::uint8_t kJd800ModelId = 0x3D;
inline constexpr std::uint8_t kJd990ModelId = 0x57;
inline constexpr std::uint8_t kRolandCmdDt1 = 0x12;
inline constexpr std::uint8_t kRolandCmdDataTransfer = 0x23;
inline constexpr std::uint8_t kRolandCmdDataTransferOneWay = 0x24;

// Patch Common
inline constexpr std::size_t kJdCommonPatchLevel = 16;
inline constexpr std::size_t kJdCommonLayerToneMask = 33;

// Patch Effect (Group A distortion drive as coarse macro)
inline constexpr std::size_t kJdEffectDistortionDrive = 11;
inline constexpr std::size_t kJdEffectReverbLevel = 44;

// Patch Tone (per-tone block)
inline constexpr std::size_t kJdToneWaveMsb = 15;
inline constexpr std::size_t kJdToneWaveLsb = 16;
inline constexpr std::size_t kJdToneLfo1Rate = 2;
inline constexpr std::size_t kJdToneLfo1Waveform = 5;
inline constexpr std::size_t kJdToneLfo1Sens = 23;
inline constexpr std::size_t kJdTonePitchEnvVelo = 27;
inline constexpr std::size_t kJdTonePitchEnvLevel0 = 30;
inline constexpr std::size_t kJdTonePitchEnvTime1 = 31;
inline constexpr std::size_t kJdTonePitchEnvLevel1 = 32;
inline constexpr std::size_t kJdTonePitchEnvTime2 = 33;
inline constexpr std::size_t kJdTonePitchEnvTime3 = 34;
inline constexpr std::size_t kJdTonePitchEnvLevel2 = 35;
inline constexpr std::size_t kJdTonePitchCoarse = 17;
inline constexpr std::size_t kJdTonePitchFine = 18;
inline constexpr std::size_t kJdToneTvfCutoff = 37;
inline constexpr std::size_t kJdToneTvfResonance = 38;
inline constexpr std::size_t kJdToneTvaLevel = 58;

inline constexpr std::size_t kJdToneTvfEnvTime1 = 47;
inline constexpr std::size_t kJdToneTvfEnvLevel1 = 48;
inline constexpr std::size_t kJdToneTvfEnvTime2 = 49;
inline constexpr std::size_t kJdToneTvfEnvLevel2 = 50;
inline constexpr std::size_t kJdToneTvfEnvTime3 = 51;
inline constexpr std::size_t kJdToneTvfEnvSustain = 52;
inline constexpr std::size_t kJdToneTvfEnvTime4 = 53;

inline constexpr std::size_t kJdToneTvaEnvTime1 = 65;
inline constexpr std::size_t kJdToneTvaEnvLevel1 = 66;
inline constexpr std::size_t kJdToneTvaEnvTime2 = 67;
inline constexpr std::size_t kJdToneTvaEnvLevel2 = 68;
inline constexpr std::size_t kJdToneTvaEnvTime3 = 69;
inline constexpr std::size_t kJdToneTvaEnvSustain = 70;
inline constexpr std::size_t kJdToneTvaEnvTime4 = 71;

inline constexpr std::size_t kJdToneBlockOffset (std::size_t toneIndex) noexcept
{
    return kJdPatchCommonBytes + kJdPatchEffectBytes + toneIndex * kJdPatchToneBytes;
}

} // namespace jdupgraded::preset
