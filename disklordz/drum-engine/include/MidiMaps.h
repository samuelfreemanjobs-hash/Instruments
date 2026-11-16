#pragma once

#include <array>
#include <cstdint>
#include <string_view>

// General MIDI percussion, with C3 = 60 (MPC / FL). C1 kick = 36, D1 snare = 38, F#1 closed hat = 42.
namespace dlz::midi {

enum class VoiceId : std::uint8_t { kick = 0, snare, clap, hat };

struct DrumVoiceInfo {
    VoiceId id;
    int gmNote;
    const char *token;
    const char *label;
};

inline constexpr std::array<DrumVoiceInfo, 4> kDrumVoices{{
    {VoiceId::kick, 36, "808", "Kick"},
    {VoiceId::snare, 38, "Snare", "Snare"},
    {VoiceId::clap, 39, "Clap", "Clap"},
    {VoiceId::hat, 42, "Hat", "Hat"},
}};

inline const DrumVoiceInfo *findVoice(const char *token) noexcept {
    if (token == nullptr)
        return nullptr;
    for (const auto &voice : kDrumVoices)
        if (std::string_view(voice.token) == token || std::string_view(voice.label) == token)
            return &voice;
    return nullptr;
}

inline constexpr int kClosedHat = 42;
inline constexpr int kOpenHat = 46;
inline constexpr int kCowbell = 56;

} // namespace dlz::midi
