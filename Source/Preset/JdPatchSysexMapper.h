#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <cstddef>
#include <cstdint>

namespace jdupgraded::preset
{

struct JdPatchSysexResult final
{
    bool checksumValid = false;
    bool patchApplied = false;
    /** Non-owning status string for logging / UI. */
    const char* status = nullptr;
};

/** Decode Roland JD-family SysEx or a raw 384-byte patch block into APVTS parameters. */
class JdPatchSysexMapper final
{
public:
    static JdPatchSysexResult applyToApvts (juce::AudioProcessorValueTreeState& apvts,
                                            const std::uint8_t* data,
                                            std::size_t size) noexcept;

    static bool applyRawPatchBlock (juce::AudioProcessorValueTreeState& apvts,
                                    const std::uint8_t* patch,
                                    std::size_t patchBytes) noexcept;

    static const std::uint8_t* findPatchPayload (const std::uint8_t* data,
                                                 std::size_t size,
                                                 std::size_t& outPayloadBytes) noexcept;
};

} // namespace jdupgraded::preset
