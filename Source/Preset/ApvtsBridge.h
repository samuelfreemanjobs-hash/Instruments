#pragma once

#include "JdPatchSysexMapper.h"

#include <juce_audio_processors/juce_audio_processors.h>

#include <array>
#include <cstdint>

namespace jdupgraded::preset
{

inline constexpr char kInternalPresetMagic[4] = { 'J', 'D', 'P', 'R' };
inline constexpr std::size_t kInternalPresetBlobBytes = 32;

/** Maps validated preset blobs and Roland SysEx to APVTS. */
class ApvtsBridge final
{
public:
    static bool applyInternalPresetBlob (juce::AudioProcessorValueTreeState& apvts,
                                         const std::uint8_t* data,
                                         std::size_t size) noexcept
    {
        if (data == nullptr || size < 8)
            return false;

        if (data[0] != kInternalPresetMagic[0] || data[1] != kInternalPresetMagic[1]
            || data[2] != kInternalPresetMagic[2] || data[3] != kInternalPresetMagic[3])
            return false;

        if (auto* master = apvts.getParameter ("masterGain"))
            master->setValueNotifyingHost (static_cast<float> (data[5]) / 127.0f);

        const char* waveIds[4] = { "tone1Wave", "tone2Wave", "tone3Wave", "tone4Wave" };
        for (int t = 0; t < 4; ++t)
        {
            if (size < static_cast<std::size_t> (8 + t))
                break;

            if (auto* param = apvts.getParameter (waveIds[t]))
                param->setValueNotifyingHost (param->convertTo0to1 (static_cast<float> (data[6 + t])));
        }

        if (size >= 14)
        {
            const char* levelIds[4] = { "tone1Level", "tone2Level", "tone3Level", "tone4Level" };
            for (int t = 0; t < 4; ++t)
            {
                if (auto* param = apvts.getParameter (levelIds[t]))
                    param->setValueNotifyingHost (static_cast<float> (data[10 + t]) / 127.0f);
            }
        }

        juce::ignoreUnused (data[4]);
        return true;
    }

    static JdPatchSysexResult applyRolandSysEx (juce::AudioProcessorValueTreeState& apvts,
                                                const std::uint8_t* data,
                                                std::size_t size) noexcept
    {
        return JdPatchSysexMapper::applyToApvts (apvts, data, size);
    }

    static void writeInternalPresetBlob (std::array<std::uint8_t, kInternalPresetBlobBytes>& out,
                                         int program,
                                         juce::AudioProcessorValueTreeState& apvts) noexcept
    {
        out.fill (0);
        out[0] = kInternalPresetMagic[0];
        out[1] = kInternalPresetMagic[1];
        out[2] = kInternalPresetMagic[2];
        out[3] = kInternalPresetMagic[3];
        out[4] = static_cast<std::uint8_t> (program & 0x7F);

        if (auto* master = apvts.getParameter ("masterGain"))
            out[5] = static_cast<std::uint8_t> (master->convertFrom0to1 (master->getValue()) * 127.0f);

        const char* waveIds[4] = { "tone1Wave", "tone2Wave", "tone3Wave", "tone4Wave" };
        for (int t = 0; t < 4; ++t)
        {
            if (auto* param = apvts.getParameter (waveIds[t]))
                out[6 + t] = static_cast<std::uint8_t> (param->convertFrom0to1 (param->getValue()));
        }

        const char* levelIds[4] = { "tone1Level", "tone2Level", "tone3Level", "tone4Level" };
        for (int t = 0; t < 4; ++t)
        {
            if (auto* param = apvts.getParameter (levelIds[t]))
                out[10 + t] = static_cast<std::uint8_t> (param->convertFrom0to1 (param->getValue()) * 127.0f);
        }
    }
};

} // namespace jdupgraded::preset
