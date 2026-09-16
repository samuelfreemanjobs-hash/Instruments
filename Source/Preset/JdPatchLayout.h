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
inline constexpr std::size_t kJdTonePitchCoarse = 17;
inline constexpr std::size_t kJdToneTvfCutoff = 37;
inline constexpr std::size_t kJdToneTvfResonance = 38;
inline constexpr std::size_t kJdToneTvaLevel = 58;

inline constexpr std::size_t kJdToneBlockOffset (std::size_t toneIndex) noexcept
{
    return kJdPatchCommonBytes + kJdPatchEffectBytes + toneIndex * kJdPatchToneBytes;
}

} // namespace jdupgraded::preset
