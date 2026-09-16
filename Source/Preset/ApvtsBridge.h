#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <cstdint>

namespace jdupgraded::preset
{

/** Maps validated internal preset blobs to APVTS (SysEx decode hooks in later phase). */
class ApvtsBridge final
{
public:
    static bool applyInternalPresetBlob (juce::AudioProcessorValueTreeState& apvts,
                                         const std::uint8_t* data,
                                         std::size_t size) noexcept
    {
        if (data == nullptr || size < 8)
            return false;

        if (data[0] != 'J' || data[1] != 'D' || data[2] != 'P' || data[3] != 'R')
            return false;

        const auto program = static_cast<int> (data[4]);
        if (auto* master = apvts.getParameter ("masterGain"))
            master->setValueNotifyingHost (static_cast<float> (data[5]) / 127.0f);

        if (auto* param = apvts.getParameter ("tone1Wave"))
            param->setValueNotifyingHost (param->convertTo0to1 (static_cast<float> (data[6])));

        if (auto* param = apvts.getParameter ("tone2Wave"))
            param->setValueNotifyingHost (param->convertTo0to1 (static_cast<float> (data[7])));

        juce::ignoreUnused (program);
        return true;
    }
};

} // namespace jdupgraded::preset
