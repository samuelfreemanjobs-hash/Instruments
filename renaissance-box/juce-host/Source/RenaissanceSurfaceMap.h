#pragma once

#include <array>

namespace renaissance
{
inline constexpr int kNumPads = 16;
inline constexpr int kPadNoteStart = 36;
inline constexpr int kQlinkCcStart = 1;
inline constexpr int kDefaultMidiChannel = 1;

/** Starting map for MPC Renaissance — verify with hardware MIDI monitor. */
class RenaissanceSurfaceMap
{
public:
    RenaissanceSurfaceMap();

    [[nodiscard]] int padNote (int padIndex) const noexcept;
    [[nodiscard]] int qlinkCc (int padIndex) const noexcept;
    [[nodiscard]] int padIndexForNote (int note) const noexcept;

    void setMidiOmni (bool omni) noexcept { midiOmni_ = omni; }
    [[nodiscard]] bool midiOmni() const noexcept { return midiOmni_; }

    void setMidiChannel (int channel1to16) noexcept;
    [[nodiscard]] int midiChannel() const noexcept { return midiChannel_; }

    [[nodiscard]] bool acceptsChannel (int midiChannel1to16) const noexcept;

private:
    std::array<int, kNumPads> padNotes_ {};
    std::array<int, kNumPads> qlinkCc_ {};
    bool midiOmni_ = true;
    int midiChannel_ = kDefaultMidiChannel;
};

} // namespace renaissance
