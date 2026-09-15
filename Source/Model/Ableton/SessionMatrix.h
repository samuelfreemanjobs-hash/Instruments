#pragma once

#include <JuceHeader.h>
#include <array>

namespace resonance::model
{
/** Ableton Live Session View: tracks × scenes clip slots. */
enum class ClipLaunchMode
{
    Trigger,
    Gate,
    Toggle,
    Repeat
};

struct SessionClip
{
    bool hasClip = false;
    juce::String clipName;
    ClipLaunchMode launch = ClipLaunchMode::Trigger;
    bool isPlaying = false;
    bool isRecording = false;
    double lengthBeats = 4.0;
    int followAction = 0; // 0 = none
};

class SessionMatrix
{
public:
    static constexpr int kTracks = 8;
    static constexpr int kScenes = 8;

    SessionClip& clip(int track, int scene) noexcept
    {
        return clips[static_cast<size_t>(track & 7)][static_cast<size_t>(scene & 7)];
    }

    const SessionClip& clip(int track, int scene) const noexcept
    {
        return clips[static_cast<size_t>(track & 7)][static_cast<size_t>(scene & 7)];
    }

    double getQuantizeBeats() const noexcept { return quantizeBeats; }
    void setQuantizeBeats(double beats) noexcept { quantizeBeats = beats; }

    bool isSessionRecord() const noexcept { return sessionRecord; }
    void setSessionRecord(bool on) noexcept { sessionRecord = on; }

    void launchScene(int sceneIndex);

private:
    std::array<std::array<SessionClip, kScenes>, kTracks> clips {};
    double quantizeBeats = 1.0; // 1 bar
    bool sessionRecord = false;
};
} // namespace resonance::model
